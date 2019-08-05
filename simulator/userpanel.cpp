#include "userpanel.h"
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QQueue>

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


/* Function to add QFormLayout:
 * Input: CircuitElement *elem
 * Action: Pull QFormLayout from panelElem and add it to panel
 * Called when element has been added in schematic.cpp
 *
void UserPanel::addForm(CircuitElement *elem) {
    QWidget *currForm = panelElem.value(elem);
    panel->addWidget(currForm);
}*/

/*Function to iterate through panelElem
 * For each CircuitElement *:
 * Store name and value into a QVector<QString>
 * Signal sendData(QVector<QString>) to circuitelement.cpp
 * | circuitelement can process the vector and store into
 * the element.
 * Void but calls schematic::parse

void getProperties() {
    QMapIterator<CircuitElement *, QWidget *> i(panelElem);
    QVector<QVector<QString>> elemProperties = new QVector;
    while (i.hasNext()) {
        i.next();
        QWidget *curr = i.value();

    }
}
*/

// ==================== SLOTS =================================
/* Adds to QMap<CircuitElement *elem, QFormLayout *formLayout>
 * based on signal from Schematic::addElement()
 * Input: CircuitElement *elem, QFormLayout *formLayout
 * Void: appends to global var QMap
 */
void UserPanel::receiveData(CircuitElement *elem){
    //QList<QLineEdit*> elemInfo;
    QWidget *userForm = new QWidget;
    QFormLayout *form = new QFormLayout;
    // User input for element name
    QLabel *nameLabel = new QLabel("Name: ");
    QLineEdit *nameLine = new QLineEdit;
    // User input for element value
    QLabel *valueLabel = new QLabel("Value: ");
    QLineEdit *valueLine = new QLineEdit;
    // Setting up panel layout
    form->addRow(nameLabel, nameLine);
    form->addRow(valueLabel, valueLine);
    userForm->setLayout(form);
    panel->addWidget(userForm);
    panelElem.insertMulti(elem, nameLine);
    panelElem.insertMulti(elem, valueLine);
    // Appending to QList for storage in QMap
    //elemInfo->append(nameLine);
    //elemInfo->append(valueLine);
    //panelElem[elem] = elemInfo;


    //QString name = nameLine->text();
    //QString value = valueLine->text();
    //panelElem[elem] = userForm;
    //addForm(elem);
}

/* Reads in user input from nameLine and valueLine
 * to pass to circuitelement::processInput | element
 * properties can be set
 * Connected to signals editingFinished()
 */

// ==================== EVENT HANDLER =========================
void UserPanel::storeInput() {
    /*
    QList<CircuitElement *> allKeys = panelElem.uniqueKeys();
    for (CircuitElement *c : allKeys) {
        QVector<QString> info;
        qDebug() << c->getName();
        QList<QLineEdit *> values;
        values.append(panelElem.value(c));
        for (QLineEdit *v : values) {
            info.append(v->text());
            qDebug() << v->text();
        }
    }*/
    //QString currKey = "";
    QList<CircuitElement*> mapKeys = panelElem.keys();
    QVector<QString> elemInput;
    for (int i = 0; i < mapKeys.length(); i++) {
        CircuitElement *currKey = mapKeys[i];
        QLineEdit *currValue = panelElem.take(currKey);
        elemInput.append(currValue->text());
        if (i != 0 && mapKeys[i] == mapKeys[i-1]) {
            //qDebug() << "Found pair: " << elemInput[0] << elemInput[1];
            mapKeys[i]->processInput(elemInput);
            elemInput.clear();
        }
    }
    //QQueue<QString> elemInput;
    /*QMapIterator<CircuitElement *, QLineEdit *> i(panelElem);
    while (i.hasNext()){
        i.next();
        QString currInput = i.value()->text();
        if (i.peekPrevious().key()) {
            if (i.peekPrevious().key() == i.key()) {
                //i.key()->processInput(elemInput);
                elemInput.append(currInput);
                elemInput.clear();
            }
        } else {
            elemInput.append(currInput);
        }
        qDebug() << i.key()->getName();
        //currKey = i.key()->getName() + i.key()->getSubtype();
        //QString currInput = i.value()->text();
        //elemInput.append(currInput);
        //elemInput.enqueue(currInput);
    }*/
}


/* Function to process user input from QFormLayout
 * Called on keyboard click of enter
 * Input: QFormLayout *formLayout, CircuitElement *elem
 * Action: Reads in user input in the form layout
 * modifies the elem, and emits signal to send elem to schematic
 * Output: CircuitElement *elem with the appropriate
 * user input for name and value
 */
