#include "simulationwizard.h"

SimulationWizard::SimulationWizard(Netlist *netlist,
                                   bool saveOnly,
                                   QWidget *parent) : QWizard(parent)
{
    this->saveOnly = saveOnly;

    if (!saveOnly) resize(500, 550);

    if (!saveOnly) {
        IntroWizardPage *introPage = new IntroWizardPage(&bcMap);
        connect(introPage, &IntroWizardPage::parseCircuit,
                this, &SimulationWizard::emitParseCircuit);
        setPage(Page_Intro, introPage);
    } else {
        setPage(Page_SaveIntro, new SaveIntroWizardPage);
    }

    setPage(Page_SimOptions, new SimOptionsWizardPage(netlist));

    setPage(Page_InitialConds, new ICWizardPage(netlist));

    setPage(Page_SaveAs, new SaveWizardPage(saveOnly, netlist));

    setPage(Page_RunSim, new SimulateWizardPage(netlist, &bcMap));

    setWindowTitle("Simulation Wizard");
    // QWizard::IndependentPages:
    // only run initializePage() once for each page.
    // Dependent upon disabled back button on simoptions
    // and sim pages.
    setOptions(QWizard::IndependentPages | QWizard::NoCancelButtonOnLastPage);
    show();
}

int SimulationWizard::nextId() const
{
    switch(currentId())
    {
    case Page_Intro:
        if (field("loadCircuit").toBool() &&
                field("completeCircuitFile").toBool())
                return Page_RunSim;
        return Page_SimOptions;

    case Page_SaveIntro:
        if (field("addSim").toBool())
            return Page_SimOptions;
        return Page_SaveAs;

    case Page_SimOptions:
        if(field("simulationType").toString() == "Transient")
            return Page_InitialConds;
        return Page_SaveAs;
    case Page_InitialConds:
        return Page_SaveAs;
    case Page_SaveAs:
        if (saveOnly) return -1;
        return Page_RunSim;

    case Page_RunSim:
    default:
        return -1;
    }
}
