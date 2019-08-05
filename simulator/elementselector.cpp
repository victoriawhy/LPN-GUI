#include "elementselector.h"

/* Constructor: ElementSelector(QWidget *parent)
 * ---------------------------------------------
 * The constructor creates a QButtonGroup to hold
 * the buttons, sets layout to a QVBoxLayout and connects buttonPressed/Released
 * signals to corresponding slots
 */
ElementSelector::ElementSelector(QWidget *parent
) : QWidget(parent)
{
    buttons = new QButtonGroup(this);
    connect(buttons, SIGNAL(buttonPressed(int)),
            this, SLOT(slotButtonPressed(int)));
    connect(buttons, SIGNAL(buttonReleased(int)),
            this, SLOT(slotButtonReleased(int)));
    deselectOnRelease = false;

    layout = new QVBoxLayout(this);
    setLayout(layout);
}

// ============ PUBLIC FUNCTIONS ===============================================

/* Public Function: addButton(const QString &, const QString &)
 * ------------------------------------------------------------
 * addButton creates a QPushButton and adds it to the
 * QButtonGroup. The buttons are currently set to be checkable
 * but this should be changed. The button is given a unique
 * integer ID which is used to associate buttons with path names
 * and element types.
 */
void ElementSelector::addButton(const QString &buttonLabel,
                                const QString &imgPath,
                                const QString &imgSelectedPath,
                                const QString &imgShadowPath,
                                bool hasLabel,
                                bool allowsExternalInput,
                                QString prefix,
                                QString units,
                                qreal widthRatio)
{
    // associate with path and element
    CircuitElement::ElementProperties elementProperties;
    elementProperties.image = QPixmap(imgPath);
    elementProperties.image = elementProperties.image.scaledToWidth(
                static_cast<int>(elementWidth * widthRatio));

    elementProperties.selected = QPixmap(imgSelectedPath);
    elementProperties.selected = elementProperties.selected.scaledToWidth(
                static_cast<int>(elementWidth * widthRatio));

    elementProperties.hasLabel = hasLabel;
    elementProperties.allowsExternalInput = allowsExternalInput;

    elementProperties.subtype = buttonLabel.toLower();
    elementProperties.prefix = prefix;
    elementProperties.units = units;

    // create button
    QPushButton *button = new QPushButton(this);
    QIcon icon(elementProperties.image);
    button->setIcon(icon);
    button->setIconSize(QSize(elementProperties.image.width() / 2,
                              elementProperties.image.height() / 2));
    button->setCheckable(true);
    button->setToolTip(buttonLabel);

    // add button to selector
    buttons->addButton(button, curId);
    layout->addWidget(button);

    shadowImagePaths[curId] = imgShadowPath;
    properties[curId] = elementProperties;
    curId++;
}

// =========== PRIVATE FUNCTIONS ===============================================

/* Slot: deselectAll()
 * --------------------
 * Deselects all buttons
 */
void ElementSelector::deselectAll()
{
    buttons->setExclusive(false);
    QAbstractButton *checked = buttons->checkedButton();
    if (checked == nullptr) return;
    checked->setChecked(false);
    buttons->setExclusive(true);
}

// ============= SLOTS =========================================================

/* Slot: slotButtonPressed(int)
 * ----------------------------
 * Check if button pressed is already selected,
 * in which case set deselectOnRelease so that
 * it is deselected when mouse is released.
 */
void ElementSelector::slotButtonPressed(int id)
{
    if (id == buttons->checkedId())
        deselectOnRelease = true;
}


/* Slot: slotButtonReleased(int)
 * -------------------------
 * Deselect all if checked button
 * is clicked.
 */
void ElementSelector::slotButtonReleased(int id)
{
    if (deselectOnRelease) {
        emit setSchematicMode(Schematic::Edit);
        deselectAll();
        deselectOnRelease = false;
        return;
    }
    emit setSchematicMode(Schematic::Build);
    emit sendElementProperties(properties[id],
                               shadowImagePaths[id]);
}
