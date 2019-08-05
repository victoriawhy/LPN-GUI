#include "savewizardpage.h"

SaveWizardPage::SaveWizardPage(bool saveOnly, Netlist *netlist, QWidget *parent) : QWizardPage(parent)
{
    this->saveOnly = saveOnly;
    this->netlist = netlist;

    // Page set-up
    setCommitPage(true);
    setTitle("Save Your Circuit");
    intro = new QLabel( this);
    intro->setWordWrap(true);
    setButtonText(QWizard::CommitButton, "&Run >");
    if (saveOnly) {
        setButtonText(QWizard::FinishButton, "Save & Exit");
    } else {
        setButtonText(QWizard::FinishButton, "Save & Start Simulation");
    }

    // Circuit Name
    nameLineEdit = new QLineEdit(this);
    registerField("circuitName*", nameLineEdit);

    // Filename & directory browser
    QWidget *browser = new QWidget(this);
    saveDirLineEdit = new QLineEdit(this);
    registerField("saveDir*", saveDirLineEdit);
    QPushButton *browseButton = new QPushButton("Browse", this);
    QHBoxLayout *browserLayout = new QHBoxLayout;

    connect(browseButton, &QPushButton::pressed,
            [=](){ saveDirLineEdit->setText( QFileDialog::getExistingDirectory(
                                                 browser,
                                                 "Choose save directory",
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                                                 )); });

    browserLayout->addWidget(saveDirLineEdit);
    browserLayout->addWidget(browseButton);
    browser->setLayout(browserLayout);

    QWidget *filename = new QWidget(this);
    filenameLineEdit = new QLineEdit(this);
    registerField("saveFilename*", filenameLineEdit);
    QLabel *ext = new QLabel(".cir", this);
    QHBoxLayout *filenameLayout = new QHBoxLayout;
    filenameLayout->addWidget(filenameLineEdit);
    filenameLayout->addWidget(ext);
    filename->setLayout(filenameLayout);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(intro);
    layout->addRow("Model name: ", nameLineEdit);
    layout->addRow("Choose directory to save circuit in: ", browser);
    layout->addRow("Save circuit as: ", filename);
    layout->setRowWrapPolicy(QFormLayout::WrapAllRows);

    connect(browseButton, &QPushButton::pressed, [this](){ emit completeChanged(); });
    connect(nameLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    connect(filenameLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    connect(saveDirLineEdit, &QLineEdit::textEdited, [this](){ emit completeChanged(); });
    setLayout(layout);
}

// ===================== PRIVATE FUNCTIONS =====================================

/* Private Function: processInput()
 * -------------------------------
 * Send simulation settings to netlist.
 * Called when the user clicks next.
 */
void SaveWizardPage::processInput()
{
    netlist->setName(field("circuitName").toString());
    netlist->setAnalysis(field("simOptions").toString());
    netlist->setInitialConditions(field("initialConditions").toString());
}

// ====================== PROTECTED FUNCTIONS ==================================

void SaveWizardPage::initializePage()
{
    if (!saveOnly && field("loadCircuit").toBool()) {
        intro->setText("Save circuit with simulation settings to a new file"
                       " or leave blank to append to existing file.");
        nameLineEdit->setEnabled(false);
    } else {
        intro->setText("Save your circuit to a file so that you can run"
                       " this simulation again without drawing a new schematic. "
                       "Once you save your circuit, you cannot edit the circuit or"
                       " simulation settings further in this wizard.");
    }
}

/* Protected Function: validatePage()
 * ---------------------------------
 * Gets confirmation from the user that
 * they're ready to save and continue to
 * the simulation page. If yes, writes
 * the netlist to file.
 */
bool SaveWizardPage::validatePage() {
    if (saveDirLineEdit->text().isEmpty() ||
            filenameLineEdit->text().isEmpty()) {
        if (saveOnly) return false;
        if (!field("loadCircuit").toBool()) return false;
        if (!saveDirLineEdit->text().isEmpty() ||
                !filenameLineEdit->text().isEmpty()) return false;
        processInput();
        netlist->appendTo(field("filename").toString());
        return true;
    }
    if (!QDir(saveDirLineEdit->text()).exists()) return false;
    if (!saveOnly && QMessageBox::question(this,
                                 "Start simulation?",
                                 "Continue?\nYou will not be able to return to "
                                           "this page",
                                 (QMessageBox::Cancel | QMessageBox::Yes))
            == QMessageBox::Cancel) return false;
    processInput();
    if (!saveOnly && field("loadCircuit").toBool()) {
            netlist->copyAndAppend(field("filename").toString(), getFilename());
    } else {
        netlist->writeToFile(getFilename());
    }
    return true;
}

/* Protected Function: isComplete()
 * -------------------------------
 * Only enable next button if a valid filename
 * is given.
 */
bool SaveWizardPage::isComplete() const {
    if (saveDirLineEdit->text().isEmpty() ||
            filenameLineEdit->text().isEmpty()) {
        if (saveOnly) return false;
        if (!field("loadCircuit").toBool()) return false;
        if (!saveDirLineEdit->text().isEmpty() ||
                !filenameLineEdit->text().isEmpty()) return false;
        return true;
    }
    return QDir(saveDirLineEdit->text()).exists();
}
