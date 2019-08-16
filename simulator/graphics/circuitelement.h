#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <QApplication>
#include <QtWidgets>

#include "node.h"
#include "../simulation/boundarycondition.h"

/* CLASS: CircuitElement
 * =====================
 * Inherits: QObject, QGraphicsItem
 * Parent: Schematic
 *
 * The CircuitElement represents a single element on the schematic. It is responsible
 * for its QPixmap (displaying the image), two nodes and a dialog box allowing
 * the user to give the element a name and value. The dialog box is opened when
 * the CircuitElement is double clicked
 *
 * To be added to a netlist, the CircuitElement must have a name of the form
 * <Prefix><Name>, two node IDs indicating its position (these are derived from
 * parsing functions and the CircuitElement class is not aware of them) and a
 * value in the default units, possibly modified by a unit modifier such as m.
 * Refer to chapters 2.1.2 and 2.1.3 of the ngspice user manual for more information
 * about naming and unit conventions in ngspice.
 *
 * Some elements (if allowsExternalInput = true) allow a filepath to a valid
 * <time> <value> formatted file as the value.
 */
class CircuitElement : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    struct ElementProperties
    {
        QPixmap image; // the normal image shown
        QPixmap selected; // red version of image, shown when selected/focused
        bool hasLabel; // if true a name/value label is displayed
        bool allowsExternalInput; // if true a filepath can be given as a value
        QString subtype; // the name of the element type e.g. Resistor
        QString prefix; // the ngspice prefix for the element (see 2.1.2)
        QString units; // default ngspice units for the element
        qreal widthRatio; // width of element relative to default width
    };

    explicit CircuitElement(
        const CircuitElement::ElementProperties properties,
        QGraphicsItem *parent = nullptr
    );
    ~CircuitElement() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    int width() { return _width; }
    int height() { return _height; }
    void rotate(qreal angle);

    void setNodes(LPN_Node *nodeOne, LPN_Node *nodeTwo);
    LPN_Node *getNodeOne() { return nodeOne; }
    LPN_Node *getNodeTwo() { return nodeTwo; }
    LPN_Node *getOtherNode(LPN_Node *node);

    // for adding to a netlist
    QString getName() { return prefix + name; }
    QString getValue() { return value + unitMod; }
    bool getAcceptExternal() { return acceptsExt; }
    QString getExternalFile() { return externalFile; }
    QString getSubtype() { return subtype; }
    void processInput(QVector<QString> elemInfo);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    // GraphicsItems
    int _width, _height;
    QPixmap display, normal, selected;
    QGraphicsSimpleTextItem *label;
    LPN_Node *nodeOne;
    LPN_Node *nodeTwo;

    // Store parameters
    QString prefix;
    QString name;
    QString value;
    QString unitMod;
    QString mu;
    QString units;
    QString externalFile;
    QString subtype;
    bool acceptsExt;

    // Dialog box
    QFormLayout *form;
    QDialog *dialogBox;
    QLineEdit *nameLineEdit;
    QLineEdit *valueLineEdit;
    QLineEdit *valueFileLineEdit;
    QComboBox *unitsComboBox;
    QList<QString> unitModifiers;
    QWidget *constValueExt;
    QWidget *extValueExt;
    bool external = false;

    // Private functions
    QFormLayout *createForm();
    QDialog *createDialogBox(QString prefix, QString units, bool allowsExternalInput);
    void setupDialog();
    void processDialogInput();


};

#endif // CIRCUITELEMENT_H
