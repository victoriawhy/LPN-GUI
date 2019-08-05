#ifndef USERPANEL_H
#define USERPANEL_H

#include <QWidget>
#include <QMap>
#include "circuitelement.h"

signals:
    void sendData(CircuitElement *);

public slots:
    void receiveData(QVector<QString>);

#endif // USERPANEL_H
