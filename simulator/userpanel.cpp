#include "userpanel.h"
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QQueue>
#include <QRadioButton>

UserPanel::UserPanel(QWidget *parent)
    :QWidget(parent)
{
    QMap<CircuitElement *, QLineEdit*> panelElem;
    panel = new QVBoxLayout(this);
    panel->setAlignment(Qt::AlignTop);
    setLayout(panel);
    out_button = new QPushButton(this);
    out_button->setText("Generate LPN file");
    out_button->resize(150, 30);
    connect(out_button, SIGNAL(released()), this, SLOT(storeInput()));
}

// ==================== SLOTS =================================
/* Adds to QMap<CircuitElement *elem, QFormLayout *formLayout>
 * based on signal from Schematic::addElement()
 * Input: CircuitElement *elem, QFormLayout *formLayout
 * Void: appends to global var QMap
 */
void UserPanel::receiveData(CircuitElement *elem) {
    if (elem->getSubtype() != "start ground" && elem->getSubtype() != "ground") {
        QLabel *nameLabel = new QLabel("Name: ");
        QLineEdit *nameLine = new QLineEdit;
        QLabel *valueLabel = new QLabel("Value: ");
        QLineEdit *valueLine = new QLineEdit;
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(nameLabel, 0, 0);
        layout->addWidget(nameLine, 0, 2, 1, 4);
        layout->addWidget(valueLabel, 1, 0);
        panelElem.insertMulti(elem, nameLine);
        if (elem->getAcceptExternal()) {
            // --- TOGGLE EXTENSIONS BUTTONS ---
            QRadioButton *constButton = new QRadioButton("Constant value");
            QRadioButton *externalButton = new QRadioButton("External value");
            QButtonGroup *valueTypeButtons = new QButtonGroup(layout);
            valueTypeButtons->addButton(constButton);
            valueTypeButtons->addButton(externalButton);
            valueTypeButtons->setExclusive(true);
            layout->addWidget(constButton, 1, 2);
            layout->addWidget(externalButton, 1, 3);

            // --- CONST VALUE OPTIONS ---
            QWidget *constValueExt = new QWidget();
            QGridLayout *constValueLayout = new QGridLayout;
            constValueLayout->addWidget(valueLine, 0, 2, 1, 2);
            constValueExt->setLayout(constValueLayout);

            // --- EXTERNAL VALUE OPTIONS ---
            QWidget *extValueExt = new QWidget();
            QWidget *browser = new QWidget;
            QHBoxLayout *browserLayout = new QHBoxLayout;
            QLineEdit *valueFileLineEdit = new QLineEdit();
            QPushButton *browseButton = new QPushButton("Browse");
            browserLayout->addWidget(valueFileLineEdit);
            browserLayout->addWidget(browseButton);
            browser->setLayout(browserLayout);
            QFormLayout *extValueLayout = new QFormLayout;
            connect(browseButton, &QPushButton::pressed,
                    [=](){ valueFileLineEdit->setText( QFileDialog::getOpenFileName(
                                                            extValueExt,
                                                            "Choose input file",
                                                            QDir::homePath(),
                                                            "All files (*.*)") ); });

            extValueLayout->addRow("Input file: ", browser);
            extValueExt->setLayout(extValueLayout);

            // --- CONNECT EXTENSIONS ---
            layout->addWidget(constValueExt, 2, 1, 1, 5);
            layout->addWidget(extValueExt, 2, 1, 1, 5);
            constValueExt->hide();
            extValueExt->hide();
            connect(constButton, &QRadioButton::toggled, [=](){
                constValueExt->setVisible(true);
                extValueExt->setHidden(true);
            });
            connect(externalButton, &QRadioButton::toggled, [=](){
                constValueExt->setHidden(true);
                extValueExt->setVisible(true);
            });
            panelElem.insertMulti(elem, valueLine);
            panelElem.insertMulti(elem, valueFileLineEdit);
        } else {
            layout->addWidget(valueLine, 1, 2, 1, 2);
            panelElem.insertMulti(elem, valueLine);
        }
        panel->addLayout(layout);
    }
}
/*
void UserPanel::receiveData(CircuitElement *elem){
    //QList<QLineEdit*> elemInfo;
    if (elem->getSubtype() != "start ground" && elem->getSubtype() != "ground") {
        QWidget *userForm = new QWidget;
        QFormLayout *form = new QFormLayout;
        QLabel *elemType = new QLabel(elem->getSubtype());
        // User input for element name
        QLabel *nameLabel = new QLabel("Name: ");
        QLineEdit *nameLine = new QLineEdit;
        form->addRow(elemType);
        form->addRow(nameLabel, nameLine);
        // User input for element value
        QLabel *valueLabel = new QLabel("Value: ");
        if (elem->getAcceptExternal()) {
            qDebug() << "can take input";
            //Change the value label field
            QRadioButton *constButton = new QRadioButton("Constant value");
            QRadioButton *externalButton = new QRadioButton("External value");
            QButtonGroup *valueTypeButtons = new QButtonGroup(form);
            valueTypeButtons->addButton(constButton);
            valueTypeButtons->addButton(externalButton);
            valueTypeButtons->setExclusive(true);
        }
        else {
            QLineEdit *valueLine = new QLineEdit;
            form->addRow(valueLabel, valueLine);
        }
        //qDebug() << (form->labelForField(nameLine));
        userForm->setLayout(form);
        //qDebug() << "parent:" << nameLabel->parentWidget();
        panel->addWidget(userForm);
        panelElem.insertMulti(elem, nameLine);
        //panelElem.insertMulti(elem, valueLine);
    }
}
*/
void UserPanel::removeForm(CircuitElement *elem) {
    for (int i=0; i<2; i++){
        QLineEdit *val = panelElem[elem];
        panel->removeWidget(val);
        QObject *obj = val->parent();
        panelElem.take(elem);
    }
}

/* Reads in user input from nameLine and valueLine
 * to pass to circuitelement::processInput | element
 * properties can be set
 * Connected to signals editingFinished()
 */

// ==================== EVENT HANDLER =========================
void UserPanel::storeInput() {
    QList<CircuitElement*> mapKeys = panelElem.keys();
    QVector<QString> elemInput;
    int emptyCount = 0;
    for (int i = 0; i < mapKeys.length(); i++) {
        CircuitElement *currKey = mapKeys[i];
        QLineEdit *currValue = panelElem.take(currKey);
        if (currValue->text() != "") {
            elemInput.append(currValue->text());
        } else {
            emptyCount++;
            /* Potential error fixing:
             * Make a new map | key:value::elementName:emptyCount
             * iterate through map and if emptyCount < 2, throw error
            if (emptyCount >= 1) {
                //Missing input for nonconstant
                if (panelElem.count(currKey) == 2) {
                    QMessageBox::warning(nullptr, "Not enough inputs", "Pls fill in the QLineEdits");
                }
                //Missing input for constant
                else{
                    if (panelElem.count(currKey) > 2) {
                        QMessageBox::warning(nullptr, "Not enough inputs", "Pls fill in the QLineEdits");
                    }
                }
            }
            */
        }
        //Complete parsing with nonconstant
        if (elemInput.size() == 2) {
            mapKeys[i]->processInput(elemInput);
            elemInput.clear();
        }

        /*elemInput.append(currValue->text());
        if (i != 0 && mapKeys[i] == mapKeys[i-1]) {
            mapKeys[i]->processInput(elemInput);
            elemInput.clear();
        }*/
    }
    emit(readyParse());
}

