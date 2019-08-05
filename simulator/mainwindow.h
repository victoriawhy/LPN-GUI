#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "schematic.h"
#include "elementselector.h"
#include "userpanel.h"
#include <QtWidgets>

/* CLASS: MainWindow
 * =================
 * Inherits: QMainWindow
 * Parent: None
 * Children:
 *  - Schematic
 *  - ElementSelector
 *
 * The MainWindow is responsible for setting up the main window with a central
 * widget (Schematic), left dock widget (ElementSelector) and toolbar.
 * The MainWindow adds buttons to the ElementSelector.
 */

namespace Ui{
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //MainWindow(QWidget *parent = nullptr);

    //Explicit declaration in an attempt to avoid ambiguity
    explicit MainWindow(QWidget *parent = nullptr);
    QString updatePanel(CircuitElement *elem);

private:
    Ui::MainWindow* ui;
    QGraphicsView *view;
    Schematic *schem; // CentralWidget - handles circuit schematic
    ElementSelector *selector; // left DockWidget - handles element menu
    UserPanel *panel; //right Dock Widget - handles user input for circuit elements


private slots:
    void addParamElem(CircuitElement *elem);
};

#endif // MAINWINDOW_H
