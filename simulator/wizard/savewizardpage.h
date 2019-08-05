#ifndef SAVEWIZARDPAGE_H
#define SAVEWIZARDPAGE_H

#include <QtWidgets>
#include "../simulation/netlist.h"

/* CLASS: SaveWizardPage
 * =====================
 * The SaveWizardPage allows the user to save their netlist.
 *
 * The user can optionally give the circuit a name, and must provide
 * a valid directory and a filename.
 *
 * When the user clicks the button to continue to the simulation, a dialogBox
 * confirms that they want to save and cannot return to editing their netlist.
 */
class SaveWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit SaveWizardPage(bool saveOnly, Netlist *netlist, QWidget *parent = nullptr);
    QString getName() {return nameLineEdit->text();}
    QString getFilename() {return saveDirLineEdit->text() + "/" + filenameLineEdit->text() + ".cir";}

protected:
    virtual bool validatePage() override;
    virtual bool isComplete() const override;
    virtual void initializePage() override;

private:
    QLabel *intro;
    QLineEdit *nameLineEdit;
    QLineEdit *saveDirLineEdit;
    QLineEdit *filenameLineEdit;

    bool saveOnly;
    Netlist *netlist;

    void processInput();
};

#endif // SAVEWIZARDPAGE_H
