#include "spiceengine.h"

/* Constructor: SpiceEngine(QObject *)
 * -----------------------------------
 * Loads ngspice library
 */
SpiceEngine::SpiceEngine(QObject *parent) : QObject(parent)
{
    lngspice = new QLibrary("ngspice", this);
}

/* Destructor: ~SpiceEngine()
 * --------------------------
 * Unloads ngspice library
 */
SpiceEngine::~SpiceEngine()
{
    lngspice->unload();
}

// =============== PUBLIC FUNCTIONS ============================================

/* Public Function: init()
 * -----------------------
 * Resolves relevant ngspice functions: ngSpice_Init, ngSpice_Init_Sync,
 * ngSpice_Command, ngSpice_running and ngSpice_CurPlot
 * Calls ngSpice_Init and ngSpiceInit_Sync to initialize the ngspice engine
 * with the callback functions
 *
 * Emits a spiceError if any library functions could not be resolved
 */
void SpiceEngine::init()
{
    InitFunction init = (InitFunction)lngspice->resolve("ngSpice_Init");
    if (!init) {
        emit spiceError("Could not load Ngspice function");
        return;
    }
    init(getchar, getstat, ng_exit, nullptr, initdata, thread_runs, this);
    InitSyncFunction initSync = (InitSyncFunction)lngspice->resolve("ngSpice_Init_Sync");
    if (!initSync) {
        emit spiceError("Could not load Ngspice function");
        return;
    }
    initSync(getvoltage, getcurrent, nullptr, nullptr, this);
    ngspice_command = (CommandFunction)lngspice->resolve("ngSpice_Command");
    ngspice_running = (RunningFunction)lngspice->resolve("ngSpice_running");
    ngspice_curPlot = (CurPlotFunction)lngspice->resolve("ngSpice_CurPlot");
    if (!ngspice_command || !ngspice_running || !ngspice_curPlot)
        emit spiceError("Could not load Ngspice function");
}

/* Public Function: getErrorStatus(QString&)
 * -----------------------------------------
 * Return errorFlag and set argument to errorMsg
 */
bool SpiceEngine::getErrorStatus(QString &message)
{
    if (errorFlag) message = errorMsg;
    return errorFlag;
}

/* Public Function: startSimulation(QString,
 *      const QMap<QString, BoundaryCondition *>, bool, QString)
 * -------------------------------------------------------------
 * Start running an ngspice background simulation with the script
 * given by filename and the boundary conditions as in bcs.
 * Dump indicates whether the output of the simulator should be
 * written to the dumpFilename.
 */
int SpiceEngine::startSimulation(QString filename,
                                  const QMap<QString, BoundaryCondition *> *bcs,
                                  bool dump, QString dumpFilename)
{
    this->dump = dump;
    this->dumpFilename = dumpFilename;
    this->bcs = bcs;
    int ret = command("source " + filename);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error loading circuit file");
        return ret;
    }
    pthread_mutex_lock(&mutex);
    ret = run();
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error running simulation");
        pthread_mutex_unlock(&mutex);
        return ret;
    }
    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}

/* Public Function: startSimulation(Netlist *, bool, QString)
 * ----------------------------------------------------------
 * Convenient version of startSimulation when schematic has
 * been parsed, since the Netlist object knows the filename
 * and has a pointer to the bcs map.
 */
int SpiceEngine::startSimulation(Netlist *netlist, bool dump, QString dumpFilename)
{
    this->dump = dump;
    this->dumpFilename = dumpFilename;
    filename = netlist->getFilename();
    this->netlist = netlist;
    int ret = command("source " + filename);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error loading circuit file");
        return ret;
    }
    pthread_mutex_lock(&mutex);
    ret = run();
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error running simulation");
        pthread_mutex_unlock(&mutex);
        return ret;
    }
    // wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}

/* Public Function: resumeSimulation()
 * -----------------------------------
 * Resume simulation in background.
 *
 * Wrapper around ngSpice_Command(bg_resume)
 * with error handling
 */
int SpiceEngine::resumeSimulation()
{
    int ret = resume();
    if (ret != 0) setErrorFlag("NGSPICE: Error resuming simulation");
    return ret;
}

/* Public Function: stopSimulation()
 * ---------------------------------
 * Stop background thread.
 *
 * Wrapper around ngSpice_Command(bg_halt)
 * with error handling
 */
int SpiceEngine::stopSimulation()
{
    if (no_bg) return 0;
    int ret = halt(); // note: halt will not return until no_bg == true
    if (ret != 0) setErrorFlag("NGSPICE: Error halting simulation");
    return ret;
}

/* Public Function: saveResults(QList<QString>, bool, QString)
 * -----------------------------------------------------------
 * Save final simulation vectors given by vecs in format
 * given by bin (true = compact binary, false = ascii), in
 * file named filename.
 */
int SpiceEngine::saveResults(QList<QString> vecs, bool bin, QString filename)
{
    QString command = "write " + filename + " ";
    foreach(QString vec, vecs) command += vec + " ";
    int ret = 0;
    if (!bin) ret = this->command("set filetype=ascii");
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error changing ngspice settings");
        return ret;
    }
    ret = this->command(command);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error saving vectors");
        return ret;
    }
    ret = this->command("set filetype=binary");
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error changing ngspice settings");
        return ret;
    }

    return 0;
}

/* Public Function: plotResults(QList<QString>, bool, QString)
 * -----------------------------------------------------------
 * Plot the given vectors using gnuplot and save in either
 * png (png == true) or eps format at the location given by
 * filename. Ngspice saves <filename>.data (containing the raw
 * data used for plotting), <filename>.eps/png (containing the plot) and
 * <filename>.plt (containing the gnuplot commands used to generate the plot).
 * If the user did not want to save these files then filename will
 * be in /tmp.
 */
int SpiceEngine::plotResults(QList<QString> vecs, bool png, QString filename)
{
    QString command = "gnuplot " + filename + " ";
    foreach(QString vec, vecs) command += vec + " ";
    int ret;
    if (png) {
        ret = this->command("set gnuplot_terminal=png");
        if (ret != 0) {
            setErrorFlag("NGSPICE: Error changing ngspice settings");
            return ret;
        }
    }
    ret = this->command(command);
    if (ret != 0) {
        setErrorFlag("NGSPICE: Error plotting vectors");
        return ret;
    }
    if (png) {
        ret = this->command("set gnuplot_terminal=eps");
        if (ret != 0) {
            setErrorFlag("NGSPICE: Error changing ngspice settings");
            return ret;
        }
    }

    return 0;

}

/* Public Function: vectors()
 * --------------------------
 * Return a list of the vectors in the current plot.
 */
QList<QString> SpiceEngine::vectors()
{
    QList<QString> vectorNames;
    foreach(pvecinfo v, vectorInfo) {
        vectorNames.append(v->vecname);
    }
    return vectorNames;
}

// ========= PUBLIC FUNCTIONS FOR USE IN CALLBACKS =============================

/* Public Function (ngspice only): _emitStatusUpdate(char *)
 * ---------------------------------------------------------
 * Emit a status update with the status of the simulation
 * if the update is a numeric % update.
 * This function is called by the ngspice callback SendStat only.
 */
void SpiceEngine::_emitStatusUpdate(char *status)
{
    QString line = QString(status);
    QStringList tokens = line.split(" ");
    if (tokens.length() < 2) {
        if (tokens[0] == "--ready--")
            emit statusUpdate(100);
        return;
    }
    qreal stat = tokens[1].left(tokens[1].length() - 1).toDouble();
    if (stat != 0.0) emit statusUpdate(static_cast<int>(stat));
}

/* Public Function (ngspice only): _writeOutput(char *)
 * ----------------------------------------------------
 * Called by the ngspice callback SendChar
 *
 * If the output is an error message (prefix stderr Error:)
 * then send a spiceError() signal.
 *
 * If dumping to file, write output to file.
 */
void SpiceEngine::_writeOutput(char *output)
{
    if (QString(output).startsWith("stderr Error:", Qt::CaseInsensitive))
        emit spiceError(QString(output));
    if (!dump) return;
    QFile file(dumpFilename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << output << endl;
        file.close();
    }
}

/* Public Function (ngspice only): _getBoundaryCondition(double *, double, char *)
 * ---------------------------------------------------------------------
 * Get boundary condition at given node and time from bcs or netlist and
 * set value of double at given address to this voltage.
 *
 * Called by ngspice callbacks GetVSRCData and GetISRCData
 */
void SpiceEngine::_getBoundaryCondition(double *value, double t, char *node)
{
    if (netlist == nullptr) {
        *value = bcs->value(node)->getState(t);
    } else {
        *value = netlist->getBoundaryValue(node, t);
    }
}

/* Public Function (ngspice only): _setVecInfo(pvecinfoall)
 * --------------------------------------------------------
 * Set values of instance variables used to display info
 * about current plot.
 *
 * Called by ngspice callback SendInitData
 */
void SpiceEngine::_setVecInfo(pvecinfoall info)
{
    plotName = QString(info->name);
    plotTitle = QString(info->title);
    plotDate = QString(info->date);
    plotType = QString(info->type);
    numVectors = info->veccount;
    vectorInfo.clear();
    for(int i = 0; i < numVectors; i++) {
        vectorInfo.append(info->vecs[i]);
    }
    emit initDataReady();
}

void SpiceEngine::_quit()
{
    command("exit");
    lngspice->unload();
}

// =============== PRIVATE FUNCTIONS ===========================================

/* Private Function: setErrorFlag(QString)
 * ---------------------------------------
 * Set errorFlag to true and errorMsg to
 * the given message
 */
void SpiceEngine::setErrorFlag(QString message)
{
    errorFlag = true;
    errorMsg = message;
}

// ============= NGSPICE CALLBACK FUNCTIONS ====================================
// More information about these callback functions can be found in section
// 19.3.3 of the NGSPICE user manual.

/* Callback Function: initdata (SendInitData)
 * ------------------------------------------
 * Callback called from bg thread in ngspice to transfer
 * initialization information.
 *
 * Sends information to spiceEngine to display in wizard
 */
int initdata(pvecinfoall intdata, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->_setVecInfo(intdata);
    return 0;
}

/* Callback function: getchar (SendChar)
 * -------------------------------------
 * Callback called from bg thread in ngspice to transfer
 * any string created by printf or puts. Output to stdout in ngspice is
 * preceded by token stdout, same with stderr.
 *
 * Sends output to spiceEngine to handle errors and write
 * to dump file.
 */
int getchar(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->_writeOutput(outputreturn);
    return 0;
}

/* Callback function: getstat (SendStat)
 * -------------------------------------
 * Callback called from bg thread to transfer status updates.
 * Emits status update through engine.
 */
int getstat(char* outputreturn, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->_emitStatusUpdate(outputreturn);
    return 0;
}

/* Callback function: thread_runs (BGThreadRunning)
 * ------------------------------------------------
 * Called from bg thread when thread stops or starts.
 * Set engine variable no_bg accordingly.
 */
int thread_runs(bool noruns, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    pthread_mutex_lock(&engine->mutex);
    engine->no_bg = noruns;
    pthread_cond_signal(&engine->cond);
    pthread_mutex_unlock(&engine->mutex);
    return 0;
}

/* Callback function: ng_exit (ControlledExit)
 * -------------------------------------------
 * Called from bg thread in ngspice if fcn controlled_exit()
 * is hit.
 * Calls engine function _quit to quit ngspice and unload library
 *
 * -- TODO --
 * Better understand what this function actually needs to do
 */
int ng_exit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{
    Q_UNUSED(ident);
    Q_UNUSED(immediate);
    Q_UNUSED(quitexit);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->_quit();
    return exitstatus;
}

/* Callback function: getvoltage (GetVSRCData)
 * -------------------------------------------
 * Called by bg thread when it needs a voltage
 * value for a voltage source with external input
 * Calls engine function _getBoundaryCondition to return
 * value.
 */
int getvoltage(double* voltage, double t, char* node, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->_getBoundaryCondition(voltage, t, node);
    return 0;
}

/* Callback function: getcurrent (GetISRCData)
 * -------------------------------------------
 * Called by bg thread when it needs a current
 * value for a current source with external input
 * Calls engine function _getBoundaryCondition to return
 * value.
 */
int getcurrent(double* current, double t, char* node, int ident, void* userdata)
{
    Q_UNUSED(ident);
    SpiceEngine *engine = static_cast<SpiceEngine *>(userdata);
    engine->_getBoundaryCondition(current, t, node);
    return 0;
}

