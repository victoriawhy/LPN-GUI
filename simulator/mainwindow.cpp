#include "mainwindow.h"
#include <QDebug>

/* Constructor: MainWindow(QWidget *parent)
 * ----------------------------------------
 * MainWindow handles the communication between the
 * schematic (drawing area) and the selector.
 *
 * The MainWindow layout has the Schematic as the CentralWidget
 * and the Selector is set as the left dockWidget.
 *
 * The MainWindow is responsible for adding buttons
 * to the Selector.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    // Create Schematic and Selector
    schem = new Schematic(this);
    selector = new ElementSelector(this);
    panel = new UserPanel(this);
    connect(schem, &Schematic::schematicClicked,
            selector, &ElementSelector::slotSchematicClicked);
    connect(selector, &ElementSelector::setSchematicMode,
            schem, &Schematic::setMode);
    connect(selector, &ElementSelector::sendElementProperties,
            schem, &Schematic::setElementProperties);
    connect(schem, SIGNAL(sendData(CircuitElement*)),
            panel, SLOT(receiveData(CircuitElement*)));
    connect(panel, SIGNAL(readyParse()),
            schem, SLOT(parseSchematic()));
    connect(schem, SIGNAL(panelRemove(CircuitElement*)),
            panel, SLOT(removeForm(CircuitElement*)));

    // Add buttons to ElementSelector
    // Each button will be associated with the
    // images and information needed for the dialog box
    QString units;
    const QChar Ohms(0x03A9);
    units.setUnicode(&Ohms, 1);
    selector->addButton("Resistor",
                        ":/images/resistor.png",
                        ":/images/resistorSelected.png",
                        ":/images/resistorShadow.png",
                        true, true, "R", units, 1);
    selector->addButton("Capacitor",
                        ":/images/capacitor.png",
                        ":/images/capacitorSelected.png",
                        ":/images/capacitorShadow.png",
                        true, true, "C", "F", 1);
    selector->addButton("Inductor",
                        ":/images/inductor.png",
                        ":/images/inductorSelected.png",
                        ":/images/inductorShadow.png",
                        true, false, "L", "H", 1);
    selector->addButton("Ground",
                        ":/images/ground.png",
                        ":/images/groundSelected.png",
                        ":/images/groundShadow.png",
                        false, false, "", "", 0.5);
    selector->addButton("Start Ground",
                        ":/images/startGround.png",
                        ":/images/startGroundSelected.png",
                        ":/images/startGroundShadow.png",
                        false, false, "", "", 0.5);
    selector->addButton("Dirichlet Surface (Pressure)",
                        ":/images/voltageSource.png",
                        ":/images/voltageSourceSelected.png",
                        ":/images/voltageSourceShadow.png",
                        true, true, "V", "V", 1);
    selector->addButton("Neumann Surface (Flow)",
                        ":/images/currentSource.png",
                        ":/images/currentSourceSelected.png",
                        ":/images/currentSourceShadow.png",
                        true, true, "I", "A", 1);
    // Set up schematic graphics view
    view = new QGraphicsView(schem, this);
    view->setMouseTracking(true);
    view->setDragMode(QGraphicsView::RubberBandDrag);

    // -- TODO --
    // The size is currently hard coded but this should
    // be changed.
    schem->setSceneRect(0, 0, 800, 800); // set initial size
    setCentralWidget(view);

    // Set up selector dock
    QDockWidget *dockSelector = new QDockWidget(this);
    dockSelector->setWidget(selector);
    dockSelector->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea,
                  dockSelector);

    QDockWidget *userInput = new QDockWidget(this);
    userInput->setWidget(panel);
    userInput->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea,userInput);

    // Set up toolbar
    QToolBar *toolbar = addToolBar("Schematic");
    toolbar->setAllowedAreas(Qt::TopToolBarArea);
    QAction *deleteButton = toolbar->addAction("Delete");
    QAction *clearButton = toolbar->addAction("Clear");
    //QAction *simulateButton = toolbar->addAction("Run simulation");
    QAction *saveButton = toolbar->addAction("Save LPN Settings");

    // Each toolbar button is connected to a Schematic method to handle
    // the click event.
    connect(deleteButton, &QAction::triggered, schem, &Schematic::deletePressed);
    connect(clearButton, &QAction::triggered, schem, &Schematic::clearPressed);
    //connect(simulateButton, &QAction::triggered, schem, &Schematic::simulatePressed);
    connect(saveButton, &QAction::triggered, schem, &Schematic::savePressed);


    //Connect schematic(child) to mainwindow(parent)
    //connect(schem, SIGNAL(elementAdded(CircuitElement*)), this, SLOT(addParamElem(CircuitElement*)));
}

QString MainWindow::updatePanel(CircuitElement *elem) {
    if (elem) {
        //qDebug() << elem->getName();
        return elem->getName();
    }
    return "elem not added";
}

// ============= SLOTS =========================================================

/* Slot: addParamElem(QSet<CircuitElement*>, CircuitElement*)
 * ----------------------------
 * Connected to signal from child schematic.cpp
 * Signal: if element is drawn
 * Takes in QSet of circuit elements and adds
 */
void MainWindow::addParamElem(CircuitElement* elem)
{
    updatePanel(elem);
}





