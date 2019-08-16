#ifndef USERPANEL_H
#define USERPANEL_H

#include <QWidget>
#include <QMap>
#include "graphics/circuitelement.h"
#include "schematic.h"

class UserPanel : public QWidget
{
    Q_OBJECT

public:
    explicit UserPanel(QWidget *parent = nullptr);
    void processInput(QList<QString> elemInfo);

private:
    QMap<CircuitElement *, QLineEdit *> panelElem;
    QVBoxLayout *panel;
    QPushButton *out_button;

signals:
    void sendData(QString name, QString value);
    void processInput(QVector<QString> elemInput);
    void readyParse();

public slots:
    void receiveData(CircuitElement *elem);
    void storeInput();
    void removeForm(CircuitElement *elem);
};
#endif // USERPANEL_H

