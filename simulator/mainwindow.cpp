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
                        true, false, "R", units, 1);
    selector->addButton("Capacitor",
                        ":/images/capacitor.png",
                        ":/images/capacitorSelected.png",
                        ":/images/capacitorShadow.png",
                        true, false, "C", "F", 1);
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
    addDockWidget(Qt::RightDockWidgetArea,
                  userInput);
    QDataWidgetMapper *mapper = new QDataWidgetMapper;
    QLineEdit *nameLineEdit = new QLineEdit;
    QLineEdit *valueLineEdit = new QLineEdit;
    mapper->addMapping(nameLineEdit, 0);
    mapper->addMapping(valueLineEdit, 1);


    //Add LPN parameter panel to right dock widget
    /*QDockWidget *paramSelector = new QDockWidget(this);

    QWidget *allParams = new QWidget;
    QVBoxLayout *vert = new QVBoxLayout;

    QWidget *naming = new QWidget;
    QFormLayout *circName = new QFormLayout;
    QLineEdit *nameLineEdit = new QLineEdit;
    circName->addRow(tr("&Circuit Name:"), nameLineEdit);
    naming->setLayout(circName);

    QWidget *resistors = new QWidget;
    QFormLayout *rLayout = new QFormLayout;
    QLineEdit *r1Edit = new QLineEdit;
    QLineEdit *r2Edit = new QLineEdit;
    QLineEdit *r3Edit = new QLineEdit;
    rLayout->addRow(tr("&Resistor 1:"), r1Edit);
    rLayout->addRow(tr("&Resistor 2:"), r2Edit);
    rLayout->addRow(tr("&Resistor 3:"), r3Edit);
    resistors->setLayout(rLayout);

    QWidget *inductors = new QWidget;
    QFormLayout *iLayout = new QFormLayout;
    QLineEdit *i1Edit = new QLineEdit;
    QLineEdit *i2Edit = new QLineEdit;
    QLineEdit *i3Edit = new QLineEdit;
    iLayout->addRow(tr("&Inductor 1:"), i1Edit);
    iLayout->addRow(tr("&Inductor 2:"), i2Edit);
    iLayout->addRow(tr("&Inductor 3:"), i3Edit);
    inductors->setLayout(iLayout);

    QWidget *capacitors = new QWidget;
    QFormLayout *cLayout = new QFormLayout;
    QLineEdit *c1Edit = new QLineEdit;
    QLineEdit *c2Edit = new QLineEdit;
    QLineEdit *c3Edit = new QLineEdit;
    cLayout->addRow(tr("&Capacitor 1:"), c1Edit);
    cLayout->addRow(tr("&Capacitor 2:"), c2Edit);
    cLayout->addRow(tr("&Capacitor 3:"), c3Edit);
    capacitors->setLayout(cLayout);

    QWidget *pressures = new QWidget;
    QFormLayout *pLayout = new QFormLayout;
    QLineEdit *p1Edit = new QLineEdit;
    QLineEdit *p2Edit = new QLineEdit;
    QLineEdit *p3Edit = new QLineEdit;
    pLayout->addRow(tr("&Pressures 1:"), p1Edit);
    pLayout->addRow(tr("&Pressures 2:"), p2Edit);
    pLayout->addRow(tr("&Pressures 3:"), p3Edit);
    pressures->setLayout(pLayout);

    QWidget *flows = new QWidget;
    QFormLayout *fLayout = new QFormLayout;
    QLineEdit *f1Edit = new QLineEdit;
    QLineEdit *f2Edit = new QLineEdit;
    QLineEdit *f3Edit = new QLineEdit;
    fLayout->addRow(tr("&Flow Source 1:"), f1Edit);
    fLayout->addRow(tr("&Flow Source 2:"), f2Edit);
    fLayout->addRow(tr("&Flow Source 3:"), f3Edit);
    flows->setLayout(fLayout);

    QLabel *rLabel = new QLabel("Resistors:", resistors);
    QLabel *iLabel = new QLabel("Inductors:", inductors);
    QLabel *cLabel = new QLabel("Capacitors:", capacitors);
    QLabel *pLabel = new QLabel("Pressure Sources:", pressures);
    QLabel *fLabel = new QLabel("Flow Sources:", flows);

    vert->addWidget(naming);
    vert->addWidget(rLabel);
    vert->addWidget(resistors);
    vert->addWidget(cLabel);
    vert->addWidget(capacitors);
    vert->addWidget(iLabel);
    vert->addWidget(inductors);
    vert->addWidget(pLabel);
    vert->addWidget(pressures);
    vert->addWidget(fLabel);
    vert->addWidget(flows);

    allParams->setLayout(vert);
    paramSelector->setWidget(allParams);
    paramSelector->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, paramSelector);
    */


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
        qDebug() << elem->getName();
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





