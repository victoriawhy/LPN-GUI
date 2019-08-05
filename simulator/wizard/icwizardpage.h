#ifndef ICWIZARDPAGE_H
#define ICWIZARDPAGE_H

#include <QtWidgets>
#include "../simulation/netlist.h"

/* Class: ICWizardPage
 * ===================
 * The ICWizardPage allows the user to set initial conditions
 * for nodes in the schematic
 *
 * This page is only displayed if Transient analysis is selected
 * on the SimOptionsWizardPage
 *
 * All fields on this page are optional, but any input provided must be
 * numeric or the Next button will be disabled
 */
class ICWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit ICWizardPage(Netlist *netlist, QWidget *parent = nullptr);

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;
    virtual void initializePage() override;

private:
    Netlist *netlist;
    QLineEdit *outputLine;
    QVBoxLayout *layout;
    QSet<QString> nodeNames;

signals:

public slots:
};

#endif // ICWIZARDPAGE_H
