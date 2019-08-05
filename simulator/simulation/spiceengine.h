#ifndef SPICEENGINE_H
#define SPICEENGINE_H

#include <stdio.h>
#include <QObject>
#include <QtWidgets>
#include "boundarycondition.h"
#include "include/sharedspice.h"
#include "netlist.h"

// Declaration of callbacks for ngspice
int getchar(char *outputreturn, int ident, void *userdata);
int getstat(char *outputreturn, int ident, void *userdata);
int ng_exit(int, bool, bool, int ident, void *);
int thread_runs(bool noruns, int ident, void *userdata);
int initdata(pvecinfoall intdata, int ident, void *userdata);
int data(pvecvaluesall vdata, int numvecs, int ident, void *userdata);
int getvoltage(double *voltage, double t, char *node, int ident, void *userdata);
int getcurrent(double *current, double t, char *node, int ident, void *userdata);

/* CLASS: SpiceEngine
 * ==================
 * Inherits: QObject
 * Parent: SimulateWizardPage
 *
 * The SpiceEngine class handles all interaction with the ngspice shared library.
 * It uses the QLibrary class to dynamically load the library and resolve methods.
 *
 * Refer to chapter 19 of the ngspice user manual to further understand the ngspice
 * shared library.
 */
class SpiceEngine : public QObject
{
    Q_OBJECT
public:
    explicit SpiceEngine(QObject *parent = nullptr);
    ~SpiceEngine();
    void init();
    int startSimulation(QString filename,
                         const QMap<QString, BoundaryCondition *> *bcs,
                         bool dump, QString dumpFilename);
    int startSimulation(Netlist *netlist, bool dump, QString dumpFilename);
    void _emitStatusUpdate(char *status);
    void _writeOutput(char *output);
    void _getBoundaryCondition(double *value, double t, char *node);
    void _quit();
    bool running() { return ngspice_running(); }
    int stopSimulation();
    int resumeSimulation();
    QString curPlot() { return QString(ngspice_curPlot()); }
    QList<QString> vectors();
    void _setVecInfo(pvecinfoall info);
    int saveResults(QList<QString> vecs, bool bin, QString filename);
    int plotResults(QList<QString> vecs, bool png, QString filename);
    bool getErrorStatus(QString &message);

    // getPlotInfo() returns a string that can be used by the
    // wizard page to display information about the current plot.
    // It simply nicely formats the information received from initialization
    // of the simulation.
    QString getPlotInfo() {
        return plotName + ": \n" +
                "Circuit: " + plotTitle + "\n" +
                "Date: " + plotDate + "\n" +
                "Plot: " + plotType; }
    bool no_bg = true; // used to determine if the background thread is running
    bool errorflag = false;
    // The mutex and condition variable are used to protect
    // no_bg and wait for it to change without blocking the
    // main thread.
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

private:
    // ngspice
    QLibrary *lngspice;
    // Typedefs for ngspice functions
    typedef int (*InitFunction)(SendChar*, SendStat*, ControlledExit*, SendData*, SendInitData*,
                        BGThreadRunning*, void*);
    typedef int (*InitSyncFunction)(GetVSRCData* , GetISRCData* , GetSyncData* , int*,
                                    void*);
    typedef int (*CommandFunction)(char*);
    typedef bool (*RunningFunction)(void);
    typedef char *(*CurPlotFunction)(void);
    // Handles for ngspice functions
    CommandFunction ngspice_command;
    RunningFunction ngspice_running;
    CurPlotFunction ngspice_curPlot;

    // convenience wrappers around ngspice functions
    int command(QString command)
    {
        return ngspice_command(const_cast<char *>(command.toLatin1().data()));
    }
    int run() { return ngspice_command(const_cast<char *>("bg_run")); }
    int halt() { return ngspice_command(const_cast<char *>("bg_halt")); }
    int resume() { return ngspice_command(const_cast<char *>("bg_resume")); }

    // Other simulation variables
    const QMap<QString, BoundaryCondition *> *bcs;
    QString filename;
    bool dump = false;
    QString dumpFilename;
    Netlist *netlist = nullptr;

    // error handling
    void setErrorFlag(QString message);
    bool errorFlag = false;
    QString errorMsg;

    // plot info
    QString plotName;
    QString plotTitle;
    QString plotDate;
    QString plotType;
    int numVectors;
    QList<pvecinfo> vectorInfo;

signals:
    void statusUpdate(int status);
    void spiceError(QString errormsg);
    void initDataReady();

public slots:
};

#endif // SPICEENGINE_H
