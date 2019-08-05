#ifndef SIMULATIONOPTIONSDIALOG_H
#define SIMULATIONOPTIONSDIALOG_H

#include <QtWidgets>
#include "../simulation/netlist.h"
#include "../simulation/spiceengine.h"
#include "savewizardpage.h"
#include "introwizardpage.h"
#include "simulatewizardpage.h"
#include "simoptionswizardpage.h"
#include "icwizardpage.h"
#include "saveintrowizardpage.h"

/* CLASS: SimulationWizard
 * =======================
 * The SimulationWizard is used for saving netlists and running simulations.
 * It creates the wizard pages and implements the logic to switch between
 * them.
 */
class SimulationWizard : public QWizard
{
    Q_OBJECT
public:
    enum { Page_Intro, Page_SaveIntro, Page_SimOptions, Page_InitialConds,
           Page_SaveAs, Page_RunSim };

    explicit SimulationWizard(Netlist *netlist,
                              bool saveOnly,
                              QWidget *parent = nullptr);

private:
    bool saveOnly;
    IntroWizardPage *introPage;
    QMap<QString, BoundaryCondition *> bcMap;

    int nextId() const override;

signals:
    void parseCircuit();
    void parseSuccess();

public slots:
    void emitParseCircuit() { emit parseCircuit(); }

};

#endif // SIMULATIONOPTIONSDIALOG_H
