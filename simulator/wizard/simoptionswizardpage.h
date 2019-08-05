#ifndef SIMOPTIONSWIZARDPAGE_H
#define SIMOPTIONSWIZARDPAGE_H

#include <QtWidgets>
#include "../simulation/netlist.h"

/* Class: SimOptionsWizardPage
 * ---------------------------
 * Allows user to select DC or Transient
 * analysis and set the relevant parameters
 * for the chosen mode. The ngspice command
 * is generated to be added to the netlist.
 */
class SimOptionsWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SimOptionsWizardPage(Netlist *netlist, QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

private:
    Netlist *netlist;
    QFormLayout *layout;
    QLineEdit *outputLine;

    QMap<QString, QString> simulationModes = {
        {"Transient", ".tran"},
        {"DC", ".dc"}
    };
    QList<QString> modes; // holds order of combobox
    QMap<int, QWidget *> simulationExtensions;
    int visibleExtension = 0;
    QString mu;
    QList<QString> unitModifiers;

    QWidget *createTranExtension();
    QWidget *createDCExtension(QSet<QString> elements);
};

#endif // SIMOPTIONSWIZARDPAGE_H
