#ifndef INTROWIZARDPAGE_H
#define INTROWIZARDPAGE_H

#include "../simulation/boundarycondition.h"
#include <QtWidgets>

/* CLASS: IntroWizardPage
 * ======================
 * The IntroWizardPage allows the user to select to use the drawn circuit diagram
 * or load a netlist from a file.
 *
 * If the user chooses to load from a file, the netlist loaded can contain only
 * a circuit, or it can contain a circuit and ngspice commands (i.e. simulation
 * commands .tran/.dc). If there are no ngspice commands found in the file, the
 * user will be taken to the relevant wizard pages to set these commands.
 * Otherwise the user will be taken directly to the SimulateWizardPage. Before
 * being taken to SimulateWizardPage, the user will be asked if they want to
 * continue since they will not be able to return.
 */
class IntroWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit IntroWizardPage(QMap<QString, BoundaryCondition *> *bcMap,
                             QWidget *parent = nullptr);

signals:
    void parseCircuit();

protected:
    virtual bool isComplete() const override;
    virtual bool validatePage() override;

private:
    QVBoxLayout *layout;
    QRadioButton *parseButton;
    QRadioButton *loadFileButton;
    QLineEdit *fileLineEdit;

    QMessageBox *conf;
    bool parseComplete = false;
    QRadioButton *completeCircuit = nullptr;

    void showExternalInputSelector();
    void hideExternalInputSelector();
    QMap<QString, BoundaryCondition *> *bcMap;
    QWidget *inputSelector = nullptr;
    QMap<QString, QLineEdit *> selectedFiles;
    void populateBoundaryConditions();

};

#endif // INTROWIZARDPAGE_H
