#ifndef NODE_H
#define NODE_H

#include <QtWidgets>

/* CLASS: Node
 * ===========
 * The Node class represents a single node (displayed as a grey circle). The
 * Node is a relatively complicated class because it must keep track of all nodes
 * it is connected to, both directly and indirectly.
 *
 * Wires are drawn as two separate lines, a vertical and a horizontal segment.
 * One segment is drawn by each node in the connection. The assignment of vertical
 * and horizontal segments is made based on the relative position of the nodes.
 * This logic could be greatly improved.
 *
 * When a node is deleted, it must ensure that no references to it remain in
 * existence. Any node it is directly or indirectly connected has a pointer to this
 * node, which must be removed. This is implemented in the destructor.
 *
 * -- TODO --
 * - The logic for drawing wires could be improved to make nicer looking schematics
 * - The logic for keeping track of connections could probably be simplified
 */
class LPN_Node : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    struct Connection
    {
        LPN_Node *node;
        QGraphicsLineItem *line;

        bool operator ==(const Connection &c1) {
            return node == c1.node;
        }
    };

public:
    enum {Type = UserType + 3};
    int type() const override {return Type;}
    explicit LPN_Node(
            QGraphicsItem *element = nullptr, // null if not attached to an element
            QGraphicsItem *parent = nullptr
            );
    ~LPN_Node() override;

    // required functions
    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    // connecting/disconnecting functions
    void connectNode(LPN_Node *node);
    void addNodes(LPN_Node *node);
    void addXNode(LPN_Node::Connection c) { xNodes.append(c); }
    void addYNode(LPN_Node::Connection c) { yNodes.append(c); }

    void removeXNode(LPN_Node::Connection c);
    void removeYNode(LPN_Node::Connection c);
    void removeNode(LPN_Node *node) { allNodes.remove(node); }

    // display functions
    void showNode();
    void hideNode();
    void displayID(int id);
    void hideID();

    QSet<LPN_Node *> getConnectedNodes();
    QSet<LPN_Node *> getAllNodesSet() { return allNodes; }
    int numConnections() { return xNodes.size() + yNodes.size(); }
    bool hasElement() { return (element != nullptr); }
    QGraphicsItem *getElement() { return element; }
    QGraphicsItem *element;
    QSet<LPN_Node *> allNodes; // all nodes this node is connected to, not only those this node draws a wire to

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    const int rad = 5;
    QVector<LPN_Node::Connection> xNodes, yNodes; // we draw horizontal segment for xNodes
                                              // and vertical segment for yNodes
    QColor line;
    QColor fill;
    QPen wire;

    QGraphicsSimpleTextItem *label = nullptr;

};

#endif // NODE_H
