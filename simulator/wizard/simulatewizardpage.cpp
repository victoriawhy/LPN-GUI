#include "simulatewizardpage.h"

SimulateWizardPage::SimulateWizardPage(Netlist *netlist,
                                       QMap<QString, BoundaryCondition *> *bcMap,
                                       QWidget *parent) : QWizardPage(parent)
{
    setTitle("Run your simulation");
    this->netlist = netlist;
    this->engine = new SpiceEngine(this);
    this->bcMap = bcMap;

    // Initial start simulation widget
    QWidget *runWidget = new QWidget(this);
    QPushButton *runButton = new QPushButton("Click to Start Simulation", this);
    QCheckBox *dumpOutputCheckbox = new QCheckBox("Write simulation output to file?", this);
    registerField("dumpOutput", dumpOutputCheckbox);
    dumpFilenameLineEdit = new QLineEdit(this);
    dumpFilenameLineEdit->setHidden(true);
    registerField("dumpFilename", dumpFilenameLineEdit);
    QVBoxLayout *runLayout = new QVBoxLayout;
    runLayout->addWidget(runButton);
    runLayout->addWidget(dumpOutputCheckbox);
    runLayout->addWidget(dumpFilenameLineEdit);
    runWidget->setLayout(runLayout);

    // Progress bar widget
    QWidget *progressWidget = new QWidget(this);
    progressBar = new QProgressBar(this);
    pauseButton = new QPushButton("Pause", this);
    QPushButton *restartButton = new QPushButton("Restart", this);
    QHBoxLayout *progressLayout = new QHBoxLayout;
    progressLayout->addWidget(progressBar, 1);
    progressBar->setRange(0, 100);
    progressLayout->addWidget(pauseButton);
    progressLayout->addWidget(restartButton);
    progressWidget->setLayout(progressLayout);
    progressWidget->setHidden(true);

    // Text about simulation
    resultsLabel = new QLabel(this);

    // SIGNALS FROM WIDGETS
    // runButton -> start the simulation and show the
    // progress widget
    connect(runButton, &QPushButton::pressed, [=](){
        runWidget->setHidden(true);
        progressWidget->setVisible(true);
        startSimulation();
    });

    // pauseButton -> pause/resume the simulation
    connect(pauseButton, &QPushButton::pressed, [=](){
        if (engine->running()) {
            stopSimulation();
            pauseButton->setText("Continue");
        } else {
            continueSimulation();
            pauseButton->setText("Pause");
        }
    });

    // restartButton -> stop and restart simulation
    connect(restartButton, &QPushButton::pressed, [=](){
        stopSimulation();
        updateStatus(0);
        startSimulation();
    });

    // dumpOutputCheckbox -> show/hide dumpFilenameLineEdit
    connect(dumpOutputCheckbox, &QCheckBox::stateChanged, [=](){
        if (dumpOutputCheckbox->isChecked()) {
            dumpFilenameLineEdit->setVisible(true);
        } else {
            dumpFilenameLineEdit->setHidden(true);
        }
    });

    // SIGNALS FROM ENGINE
    connect(this->engine, &SpiceEngine::statusUpdate,
            this, &SimulateWizardPage::updateStatus,
            Qt::QueuedConnection);

    connect(this->engine, &SpiceEngine::spiceError,
            this, &SimulateWizardPage::receiveError,
            Qt::QueuedConnection);

    connect(this->engine, &SpiceEngine::initDataReady,
            this, &SimulateWizardPage::initData,
            Qt::QueuedConnection);
    engine->init();

    layout = new QVBoxLayout;
    layout->addWidget(runWidget);
    layout->addWidget(progressWidget);
    layout->addWidget(resultsLabel);
    setLayout(layout);
}

// ================= PRIVATE FUNCTIONS =========================================

/* Private Function: showErrorMessage()
 * ------------------------------------
 * Get error message from engine and show
 * in a critical messagebox
 */
void SimulateWizardPage::showErrorMessage()
{
    QString message;
    if (engine->getErrorStatus(message))
        QMessageBox::critical(this, "Simulation Error", message);
}

/* Private Function: startSimulation()
 * -----------------------------------
 * Start the simulation with the given
 * circuit. Make sure the finish, plot and save
 * buttons are disabled.
 */
void SimulateWizardPage::startSimulation()
{
    if (plotButton && plotButton->isEnabled()) {
        plotButton->setEnabled(false);
        saveButton->setEnabled(false);
    }
    int ret;
    if (field("loadCircuit").toBool()) {
        // TODO: allow user to provide filename for any External input elements
        // create bcs map, and pass a pointer to this function.
        ret = engine->startSimulation(field("filename").toString(),
                                          bcMap,
                                          field("dumpOutput").toBool(),
                                          field("dumpFilename").toString());
    } else {
        ret = engine->startSimulation(netlist,
                                      field("dumpOutput").toBool(),
                                      field("dumpFilename").toString());
    }
    if (ret != 0) showErrorMessage();

    emit completeChanged();
}

/* Private Function: continueSimulation()
 * --------------------------------------
 * Resume simulation after a pause. Disable
 * finish, plot and save buttons.
 */
void SimulateWizardPage::continueSimulation()
{
    plotButton->setEnabled(false);
    saveButton->setEnabled(false);
    int ret = engine->resumeSimulation();
    if (ret != 0) showErrorMessage();

    emit completeChanged();
}

/* Private Function: stopSimulation()
 * ----------------------------------
 * Pause simulation. Enable finish, plot and
 * save buttons.
 */
void SimulateWizardPage::stopSimulation()
{
    int ret = engine->stopSimulation();
    if (ret != 0) showErrorMessage();
    plotButton->setEnabled(true);
    saveButton->setEnabled(true);

    emit completeChanged();
}

/* Private Function: getPlotWidget()
 * ---------------------------------
 * Create and return the plot options
 * widget.
 */
QWidget *SimulateWizardPage::getPlotWidget()
{
    QWidget *plotWidget = new QWidget;
    QVBoxLayout *plotLayout = new QVBoxLayout;

    // save question
    QHBoxLayout *saveLayout = new QHBoxLayout;
    saveLayout->addWidget(new QLabel("Save plot? ", this));
    QButtonGroup *saveButtons = new QButtonGroup(this);
    QRadioButton *yesButton = new QRadioButton("Yes", this);
    yesButton->setChecked(true);
    registerField("savePlot", yesButton);
    saveButtons->addButton(yesButton);
    saveLayout->addWidget(yesButton);
    QRadioButton *noButton = new QRadioButton("No", this);
    saveButtons->addButton(noButton);
    saveLayout->addWidget(noButton);
    saveLayout->addStretch(1);


    // save options
    QWidget *saveOptionsWidget = new QWidget(plotWidget);
    QVBoxLayout *saveOptionsLayout = new QVBoxLayout;
    connect(yesButton, &QRadioButton::toggled, saveOptionsWidget, &QWidget::setVisible);
    connect(noButton, &QRadioButton::toggled, saveOptionsWidget, &QWidget::setHidden);

    // file name
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(new QLabel("Save as:", this));
    QLineEdit *filenameLineEdit = new QLineEdit(defaultFilename, this);
    registerField("plotFilename", filenameLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    filenameLayout->addWidget(filenameLineEdit, 1);
    filenameLayout->addWidget(browseButton);

    connect(browseButton, &QPushButton::pressed,
            [=](){ filenameLineEdit->setText( QFileDialog::getOpenFileName(
                                                 saveOptionsWidget,
                                                 "Choose file",
                                                 QDir::homePath(),
                                                  "All files")); });
    // file type
    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(new QLabel("Format:"));
    QComboBox *plotFormat = new QComboBox(this);
    plotFormat->addItems({"PNG", "Postscript"});
    registerField("plotFormat", plotFormat, "currentText");
    formatLayout->addWidget(plotFormat);
    formatLayout->addStretch(1);

    saveOptionsLayout->addLayout(filenameLayout);
    saveOptionsLayout->addLayout(formatLayout);
    saveOptionsWidget->setLayout(saveOptionsLayout);

    // plot button
    plotButton = new QPushButton("Plot", this);
    plotButton->setEnabled(false);
    connect(plotButton, &QPushButton::released, this, &SimulateWizardPage::plot);

    // overall layout
    plotLayout->addLayout(saveLayout);
    plotLayout->addWidget(saveOptionsWidget);
    plotLayout->addWidget(plotButton, 0, Qt::AlignCenter);

    plotWidget->setLayout(plotLayout);
    return plotWidget;
}

/* Private Function: getSaveWidget()
 * ---------------------------------
 * Create and return save options widget
 */
QWidget *SimulateWizardPage::getSaveWidget()
{
    QWidget *saveWidget = new QWidget;
    QVBoxLayout *saveLayout = new QVBoxLayout;

    // filename
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(new QLabel("Save as:", this));
    QLineEdit *filenameLineEdit = new QLineEdit(defaultFilename + "_out.raw", this);
    registerField("saveResultsFilename", filenameLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    filenameLayout->addWidget(filenameLineEdit, 1);
    filenameLayout->addWidget(browseButton);

    connect(browseButton, &QPushButton::pressed,
            [=](){ filenameLineEdit->setText( QFileDialog::getOpenFileName(
                                                 saveWidget,
                                                 "Choose file",
                                                 QDir::homePath(),
                                                  "All files")); });
    // format
    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(new QLabel("Format:"));
    QComboBox *saveFormat = new QComboBox(this);
    saveFormat->addItems({"Compact Binary", "ASCII"});
    registerField("saveFormat", saveFormat, "currentText");
    formatLayout->addWidget(saveFormat);
    formatLayout->addStretch(1);

    // save button
    saveButton = new QPushButton("Save", this);
    saveButton->setEnabled(false);
    connect(saveButton, &QPushButton::released, this, &SimulateWizardPage::save);

    // layout
    saveLayout->addLayout(filenameLayout);
    saveLayout->addLayout(formatLayout);
    saveLayout->addWidget(saveButton, 0, Qt::AlignCenter);
    saveWidget->setLayout(saveLayout);
    return saveWidget;
}

/* Private Function: save()
 * ------------------------
 * Save the selected vectors to file.
 * Calls ngspice command "save".
 * If no vectors are selected the user is
 * offered the choice to save all or cancel.
 */
void SimulateWizardPage::save()
{
    QList<QString> toWrite;
    for (int i = 0; i < selectedVectors.size(); i++) {
        if (selectedVectors.values()[i])
            toWrite.append(selectedVectors.keys()[i]);
    }
    if (toWrite.empty()) {
        int ret = QMessageBox::question(this,
                                        "Save all?",
                                        "No vectors selected. Save all vectors?",
                                        (QMessageBox::Yes | QMessageBox::Cancel));
        if (ret == QMessageBox::Cancel) return;
    }

    saveButton->setText("Saving...");
    update();
    QApplication::processEvents();
    int ret = engine->saveResults(toWrite,
                                  field("saveFormat").toString() == "Compact Binary",
                                  field("saveResultsFilename").toString());
    if (ret != 0) {
        showErrorMessage();
    } else {
        QMessageBox::information(this,
                                 "Save successful",
                                 "Your results have been saved!");
    }

    saveButton->setText("Save");
}

/* Private Function: plot()
 * ------------------------
 * Plot the selected vectors. Optionally save results
 * to file. If no vectors are selected, show user
 * an error message and cancel.
 */
void SimulateWizardPage::plot()
{
    QList<QString> toPlot;
    for (int i = 0; i < selectedVectors.size(); i++) {
        if (selectedVectors.values()[i])
            toPlot.append(selectedVectors.keys()[i]);
    }
    if (toPlot.empty()) {
        QMessageBox::critical(this,
                              "Error",
                              "No vectors selected. "
                              "Check the vectors you want to plot.");
        return;
    }

    plotButton->setText("Loading plot...");
    update();
    QApplication::processEvents();

    QString filename = (field("savePlot").toBool() ?
                            field("plotFilename").toString() :
                            QDir::toNativeSeparators(QDir::tempPath() + "/gnuout"));
    if (filename.isEmpty()) filename = QDir::toNativeSeparators(QDir::tempPath() + "/gnuout");
    int ret = engine->plotResults(toPlot,
                                  field("plotFormat").toString() == "PNG",
                                  filename);
    if (ret != 0) showErrorMessage();

    plotButton->setText("Plot");
}

/* Private Function: initData()
 * ----------------------------
 * Print simulaiton init information to the wizard page
 * Add vectors with checkboxes
 * Add tab layout with plot and save options
 */
void SimulateWizardPage::initData()
{
    disconnect(engine, &SpiceEngine::initDataReady,
            this, &SimulateWizardPage::initData);

    resultsLabel->setText(engine->getPlotInfo() + "\n\nVectors:");
    vectors = engine->vectors();
    for (int i = 0; i < vectors.length(); i++) {
        if (vectors[i] == "time") continue;
        QCheckBox *vectorCheckbox = new QCheckBox("    " + vectors[i], this);

        connect(vectorCheckbox, &QCheckBox::stateChanged, [=](int state){
            selectedVectors[vectors[i]] = (state == Qt::Checked);
        });

        layout->addWidget(vectorCheckbox, i);
    }

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(getPlotWidget(), "Plot");
    tabs->addTab(getSaveWidget(), "Save");

    layout->addWidget(tabs);
}

// =============== PROTECTED FUNCTIONS =========================================

/* Protected Function: initializePage()
 * ------------------------------------
 * Get the default filename from the wizard fields
 * to be used as save/plot filename suggestions.
 * Set dump filename suggestion.
 */
void SimulateWizardPage::initializePage()
{
    QString filename;
    if (field("loadCircuit").toBool()) {
        filename = field("filename").toString();
    } else {
        filename = netlist->getFilename();
    }
    defaultFilename = filename.left(filename.length() - 4);
    dumpFilenameLineEdit->setText(defaultFilename + "_dump.txt");
}

/* Protected Function: isComplete()
 * --------------------------------
 * Enable Finish button only if simulation
 * is paused or complete
 *
 * For exit by error or (x) button the
 * Schematic calls engine->stopSimulation()
 * while cleaning up simulation.
 */
bool SimulateWizardPage::isComplete() const
{
    if (!engine->running()) return true;
    return (progressBar->value() == 100);
}

// ====================== SLOTS ================================================

/* Slot: updateStatus(int)
 * -----------------------
 * Gets progress from engine and updates
 * progress bar.
 * Handles complete simulations
 */
void SimulateWizardPage::updateStatus(int progress)
{
    progressBar->setValue(progress);
    if (progress == 100) {
        plotButton->setEnabled(true);
        saveButton->setEnabled(true);
        pauseButton->setEnabled(false);
    } else {
        plotButton->setEnabled(false);
        saveButton->setEnabled(false);
        pauseButton->setEnabled(true);
    }
    emit completeChanged();
}

/* Slot: receiveError(char *)
 * --------------------------
 * Display error received from engine in a critical message popup
 */
void SimulateWizardPage::receiveError(QString errormsg){
    qInfo() << errormsg;
    QMessageBox::critical(this, "Simulation Error", errormsg);
    wizard()->close();
}
