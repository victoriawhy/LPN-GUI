#ifndef ELEMENTSELECTOR_H
#define ELEMENTSELECTOR_H

#include <QtWidgets>
#include "schematic.h"

/* CLASS: ElementSelector
 * ======================
 * Inherits: QWidget
 * Parent: MainWindow
 *
 * The ElementSelector's main purpose is to manage the buttons allowing the user
 * to add elements to the schematic. The ElementSelector uses an exclusive
 * QButtonGroup with checkable QPushButtons.
 *
 * To allow for all buttons to be unchecked, the ElementSelector implements
 * custom logic to turn off the QButtonGroup's exclusivity, unselect all
 * buttons, then make the QButtonGroup exclusive again. (see deselectAll())
 */

class ElementSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ElementSelector(QWidget *parent = nullptr);
    /* Public Function: addButton
     * --------------------------
     * buttonLabel: displayed as the button's tooltip
     * imgPath: path to the image shown on the button and the image shown
     *          when the element is placed on the schematic
     * imgSelectedPath: path to red version of the image, shown when the element
     *                  is selected
     * imgShadowPath: path to grey version of the image, shown when the element
     *                is being dragged around before it's placed
     * hasLabel: if true, the element should display a label with name and value
     * allowsExternalInput: if true, the value can be a path to a file containing
     *                      values
     * prefix: Ngspice prefix for the element (e.g. V for voltage source, R for
     *         resistor)
     * units: Default ngspice units for the element (e.g. F for capacitor)
     * widthRatio: width of the element as a fraction of the defined elementWidth
     */
    void addButton(const QString &buttonLabel,
                   const QString &imgPath,
                   const QString &imgSelectedPath,
                   const QString &imgShadowPath,
                   bool hasLabel,
                   bool allowsExternalInput,
                   QString prefix,
                   QString units,
                   qreal widthRatio);

protected:
    // mousePressEvent reimplementation allows for the user
    // to click on the dock outside of a button and deselect
    // all the buttons
    void mousePressEvent(QMouseEvent *) { deselectAll(); }

private:
    QVBoxLayout *layout;
    QButtonGroup *buttons;
    QMap<int, CircuitElement::ElementProperties> properties;
    // ShadowImagePaths are associated to buttons separately from other
    // properties, since the CircuitElement object never needs to know
    // about the shadowImage. Once the element is placed (CircuitElement
    // constructed), then the shadowImage is never used again.
    QMap<int, QString> shadowImagePaths;
    int curId = 1;
    bool deselectOnRelease;
    // -- TODO --
    // This is somewhat arbitrary. It would nice to have this
    // be adaptable to window size or manually adjustable.
    const qreal elementWidth = 160.0;

    void deselectAll();

signals:
    void setSchematicMode(Schematic::Mode mode);
    void sendElementProperties(CircuitElement::ElementProperties properties,
                               QString path);

public slots:
    void slotButtonPressed(int id);
    void slotButtonReleased(int id);
    // When the user clicks on the schematic, deselect all the buttons
    void slotSchematicClicked() { deselectAll(); }
};

#endif // ELEMENTSELECTOR_H
