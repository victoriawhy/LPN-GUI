#include "netlist.h"
#include <fstream>
#include <QDebug>

/* Constructor */
Netlist::Netlist(const QString &name, QObject *parent) : QObject(parent)
{
    this->setName(name);
}

// ================== PUBLIC FUNCTIONS =========================================

/* Public Function: addElement(CircuitElement *, int, int)
 * -------------------------------------------------------
 * Add element using given CircuitElement object and nodes
 */
int Netlist::addElement(CircuitElement *element, int nodeIn, int nodeOut)
{
    if (element->getValue() == "" && element->getExternalFile() == "")
        return NoValueError;
    if (element->getName().length() == 1) return NoNameError;
    if (elementNames.contains(element->getName())) return DuplicateNameError;

    if (element->getName().front() == "V" && nodeIn == 0) {
        nodeIn = nodeOut;
        nodeOut = 0;
    }
    QString line = element->getName() + " " + QString::number(nodeIn) + " " +
            QString::number(nodeOut);
    if (element->getExternalFile() != "") {
        line += " external";
        BoundaryCondition *bc = new BoundaryCondition(element->getExternalFile(),
                                                      this);
        boundaryConditions[element->getName().toLower()] = bc;
    } else {
        line += (" " + element->getValue());
    }

    elementNames.insert(element->getName());
    nodeNames.insert(QString::number(nodeIn));
    nodeNames.insert(QString::number(nodeOut));
    elements.append(line);
    return 0;
}

/* Public Function: groundElement(CircuitElement *)
 * ------------------------------------------------
 * Connect the given element to ground (change second
 * node to 0)
 */
int Netlist::groundElement(CircuitElement *element)
{
    if (!elementNames.contains(element->getName())) return -1;
    for (QString line : elements) {
        if (line.split(" ")[0] != element->getName()) continue;
        elements.removeOne(line);
        QStringList tokens = line.split(" ");
        nodeNames.remove(tokens[2]);
        tokens[2] = QString::number(0);
        elements.append(tokens.join(" "));
    }
    return 0;
}

/* Public Function: writeToFile(const QString &)
 * ---------------------------------------------
 * Write netlist to file as a script that can be
 * run by ngspice.
 */
void Netlist::writeToFile(const QString &filename)
{
    QFile file(filename);
    //QString testFilename = "/Users/vyuan/Desktop/LPN-Plugin-master/output_files/testOutput.txt";
    //QFile testFile(testFilename);
    //QFileInfo fi(testFile);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        // name
        out << name << endl;
        // elements
        for (QString elem : elements) {
            out << elem << endl;
        }
        // ics
        for (QString ic : initialConditions) {
            out << ic << endl;
        }
        // analysis
        out << analysis << endl;
        // end

        if (!analysis.isEmpty()) {
            out << ".end" << endl;
        }
        file.close();
    }
    this->filename = filename;
}

/* Public Function: appendTo(const QString &)
 * ------------------------------------------
 * Append simulation settings to an existing
 * file. Assumes the file contains only name
 * and circuit elements.
 */
void Netlist::appendTo(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly | QIODevice::Text | QFile::Append)) {
        QTextStream out(&file);
        out << endl;
        // ics
        for (QString ic : initialConditions){
            out << ic << endl;
        }
        // analysis
        out << analysis << endl;
        // end
        out << ".end" << endl;
        file.close();
    }
    this->filename = filename;
}

/* Public Function: copyAndAppend(const QString&, const QString&)
 * --------------------------------------------------------------
 * Copy text from the file called existingFilename to a new file
 * called newFilename, then append simulation settings to the
 * new file. Assumes the exisiting file contains only name and
 * circuit elements.
 */
void Netlist::copyAndAppend(const QString &newFilename,
                            const QString &existingFilename)
{
    QFile newFile(newFilename);
    QFile existingFile(existingFilename);

    //Testing to make output file
    QString testFilename = "test.txt";
    QFile testFile(testFilename);

    if (!newFile.open(QFile::WriteOnly | QIODevice::Text)) return;
    if (!existingFile.open(QFile::ReadOnly | QIODevice::Text)) return;
    if (!testFile.open(QFile::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&newFile);
    QTextStream in(&existingFile);
    QTextStream test(&testFile);

    while(!in.atEnd()) {
        out << in.readLine() << endl;
    }
    // ics
    for (QString ic : initialConditions) {
        out << ic << endl;
    }
    // analysis
    out << analysis << endl;
    // end of file
    out << ".end" << endl;
    newFile.close();
    existingFile.close();
    this->filename = newFilename;
    //this->filename = testFilename;

}

// =============== STATIC PUBLIC METHODS =======================================

QSet<QString> Netlist::parseNodesFromFile(const QString &filename)
{
    QSet<QString> nodes;
    QFile file(filename);
    if (file.open(QFile::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        if (!in.atEnd()) in.readLine(); // title line
        while(!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith(".") || line.startsWith("*")) continue;
            QStringList tokens = line.split(QRegExp("\\s+"));
            if (tokens.length() < 3) continue;
            nodes.insert(tokens[1]);
            nodes.insert(tokens[2]);
        }
    }
    return nodes;
}
