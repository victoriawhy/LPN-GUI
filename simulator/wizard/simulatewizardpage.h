#ifndef SIMULATEWIZARDPAGE_H
#define SIMULATEWIZARDPAGE_H

#include <QtWidgets>
#include "../simulation/netlist.h"
#include "../simulation/spiceengine.h"

/* CLASS: SimulateWizardPage
 * =========================
 * SimulateWizardPage is responsible for all user interaction with the simulation
 * engine.
 * The user can start the simulation, pause/resume/restart the simulation as
 * desired and plot and save results at any time when the simulation is finished
 * or paused.
 * The vectors/initialization information from the ngspice engine is displayed
 * when available.
 */

class SimulateWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    // The netlist is provided if the netlist was parsed from the schematic
    // (as opposed to loaded from a file). The bcMap is provided if any elements
    // have external input. Else these arguments are null.
    explicit SimulateWizardPage(Netlist *netlist = nullptr,
                                QMap<QString, BoundaryCondition *> *bcMap = nullptr,
                                QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual void initializePage() override;

private:
    SpiceEngine *engine = nullptr;
    Netlist *netlist = nullptr;
    QMap<QString, BoundaryCondition *> *bcMap;

    QVBoxLayout *layout = nullptr;
    QProgressBar *progressBar = nullptr;
    QLineEdit *dumpFilenameLineEdit = nullptr;
    QLabel *resultsLabel = nullptr;
    QPushButton *saveButton = nullptr;
    QPushButton *plotButton = nullptr;
    QPushButton *pauseButton = nullptr;
    bool running;
    bool resultsInitialized = false;
    QList<QString> vectors;
    QMap<QString, bool> selectedVectors;
    QString defaultFilename;

    void showErrorMessage();

    void startSimulation();
    void continueSimulation();
    void stopSimulation();
    void showResults();
    void initData();
    void writeVectors();
    void plot();
    void save();

    QWidget *getPlotWidget();
    QWidget *getSaveWidget();

    void parseBoundaryConditions(QString filename,
                                 QMap<QString, BoundaryCondition *> *bcMap);
    QString getFile(QString node);

private slots:
    void updateStatus(int progress);
    void receiveError(QString errormsg);
};

#endif // SIMULATEWIZARDPAGE_H
