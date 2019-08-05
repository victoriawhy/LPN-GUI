#-------------------------------------------------
#
# Project created by QtCreator 2018-07-06T15:41:07
#
#-------------------------------------------------

QT       += core gui multimedia network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simulator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=  \
        main.cpp \
        mainwindow.cpp \
    schematic.cpp \
    elementselector.cpp \
    graphics/circuitelement.cpp \
    graphics/node.cpp \
    simulation/netlist.cpp \
    simulation/boundarycondition.cpp \
    simulation/spiceengine.cpp \
    wizard/simulationwizard.cpp \
    wizard/savewizardpage.cpp \
    wizard/introwizardpage.cpp \
    wizard/simulatewizardpage.cpp \
    wizard/simoptionswizardpage.cpp \
    wizard/icwizardpage.cpp \
    userpanel.cpp

HEADERS += \
        mainwindow.h \
    schematic.h \
    elementselector.h \
    graphics/circuitelement.h \
    graphics/node.h \
    simulation/netlist.h \
    simulation/boundarycondition.h \
    simulation/spiceengine.h \
    wizard/simulationwizard.h \
    wizard/savewizardpage.h \
    wizard/introwizardpage.h \
    wizard/simulatewizardpage.h \
    wizard/simoptionswizardpage.h \
    wizard/icwizardpage.h \
    wizard/saveintrowizardpage.h \
    userpanel.h

RESOURCES += \
    images.qrc

FORMS += \
    mainwindow.ui
