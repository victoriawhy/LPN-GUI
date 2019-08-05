#ifndef NETLIST_H
#define NETLIST_H

#include <QtWidgets>
#include "../graphics/circuitelement.h"
#include "boundarycondition.h"

/* CLASS: Netlist
 * ==============
 * The Netlist class represents an ngspice compatible netlist. The ngspice
 * manual has extensive explanations of the ngspice scripting language. It
 * may help to understand to understand the structure of the netlist.
 *
 * This class currently only allows for very simple netlists. The general
 * structure is:
 *
 * <Title>
 *
 * <Element line>
 * ...
 * <Element line>
 *
 * .ic <initial conditions>
 * [.tran OR .dc] <simulation settings> [uic (.tran only)]
 * .end
 *
 * where <Title> is any string, an element line is of the form:
 * <Prefix><Name> <node 1> <node 2> [<value>(<unit modifier>) OR external],
 * an initial conditions line is of the form:
 * <node>=<value> ... <node>=<value>
 * and simulation settings are either:
 * .tran <timestep> <total time> uic
 * or:
 * .dc <source> <start voltage> <stop voltage>
 *
 * The title is set by the SaveWizardPage
 * The elements are added from the Schematic
 * The initial conditions are set by the ICWizardPage
 * The simulation settings are set by the SimOptionsWizardPage
 *
 * Currently the netlist must be written to a file to be used in a simulation.
 * This could be changed by implementing functions that yield either a list
 * of QStrings that could by used with the ngspice command "circbyline" or
 * a char** to be used with the exported function ngSpice_Circ(). See section
 * 19.4.1 of the ngspice manual for more information about loading a netlist.
 */
class Netlist : public QObject
{
    Q_OBJECT
public:
    explicit Netlist(const QString &name = "", QObject *parent = nullptr);
    ~Netlist() {}

    /* Errors:
     * -------
     * All elements in an ngspice netlist must have the format
     * <Prefix><Name> <node> <node> <value>(<unit modifier>)
     * Further all <Prefix><Name> combinations must be unique
     * If these rules are violated an error will be returned during
     * parsing
     *
     * NoValueError: An element was found that did not have a value
     * NoNameError: An element was found that did not have a name
     * DuplicateNameError: An element was found that had the same name as an
     *                     element already added to the netlist.
     */
    enum { NoValueError = 3, NoNameError = 4, DuplicateNameError = 5 };

    // Editing functions
    int addElement(CircuitElement *element, int nodeIn, int nodeOut);
    int groundElement(CircuitElement *element);
    void setName(const QString &name) { this->name = name; }
    void setFilename(const QString &filename) { this->filename = filename; }
    void setAnalysis(QString line) { analysis = line; }
    void setInitialConditions(QString line) { initialConditions.append(line); }

    // Exporting functions
    void writeToFile(const QString &filename);
    void appendTo(const QString &filename);
    void copyAndAppend(const QString &newFilename,
                       const QString &existingFilename);

    // Getters
    QString getFilename() { return filename; }
    QSet<QString> getElementNames() { return elementNames; }
    QSet<QString> getNodeNames() { return nodeNames; }

    // External Boundary Condition Handling
    QMap<QString, BoundaryCondition *> boundaryConditions;
    double getBoundaryValue(char *node, double time)
    {
        return boundaryConditions[node]->getState(time);
    }
    const QMap<QString, BoundaryCondition *> *getBoundaryConditions()
    {
        return &boundaryConditions;
    }

    // Static function: Read netlist file to get nodes
    static QSet<QString> parseNodesFromFile(const QString &filename);

private:
    QString name;
    QVector<QString> elements;
    QSet<QString> elementNames, nodeNames;
    QVector<QString> initialConditions;
    QString analysis;
    QString filename;
    QString graphingCommand;
};

#endif // NETLIST_H
