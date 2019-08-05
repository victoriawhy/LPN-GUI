#include "boundarycondition.h"

/* Constructor: BoundaryCondition(QString, QObject *)
 * -------------------------------------------------
 * Create a new BoundaryCondition object with values
 * given in the file named by argument filename.
 *
 * Emits signal badFile() if file is not properly
 * formatted.
 *
 * Period is determined to be the largest time value
 * given in the file plus one time step
 */
BoundaryCondition::BoundaryCondition(QString filename,
                                     QObject *parent) : QObject(parent)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    qreal maxTime = 0;
    qreal step = 0;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList tokens = line.split(QRegExp("\\s+"));
        if (tokens.length() != 2) {
            emit badFile();
            return;
        }
        bool ok;
        qreal time = tokens[0].toDouble(&ok);
        if (!ok) {
            emit badFile();
            return;
        }
        step = time - maxTime;
        maxTime = time > maxTime ? time : maxTime;
        states[time] = tokens[1].toDouble(&ok);
        if (!ok) {
            emit badFile();
            return;
        }
    }
    this->period = maxTime + step;
}

// ============== STATIC METHODS ===============================================

/* Static method: checkFile(QString)
 * ---------------------------------
 * Returns true if file is properly formatted
 * i.e. every line is of the form <time>\t<value>
 * where <time> and <value> are numeric.
 */
 bool BoundaryCondition::checkFile(QString filename)
{
     QFile file(filename);
     if (!file.open(QFile::ReadOnly | QIODevice::Text)) return false;
     QTextStream in(&file);
     bool ok = true;
     while(!in.atEnd()){
         QStringList tokens = in.readLine().split(QRegExp("\\s+"));
         if (tokens.length() != 2) {
             ok = false;
             break;
         }
         tokens[0].toDouble(&ok);
         if (!ok) break;
         tokens[1].toDouble(&ok);
         if (!ok) break;
     }
     file.close();
     return ok;
}

// ================= PUBLIC ====================================================

/* Public Function: getState(double)
 * ---------------------------------
 * Returns the voltage at the given time
 */
double BoundaryCondition::getState(double time)
{
    QMap<qreal, qreal>::iterator high, low;
    time = fmod(time, period);
    high = states.lowerBound(time);
    low = (high == states.begin() ? high : high--);

    return interpolate(low, high, time);
}

// ================= PRIVATE ===================================================

/* Private Function: interpolate(QMap<qreal, qreal>::iterator,
 * QMap<qreal, qreal>::iterator, double)
 * -----------------------------------------------------------
 * Return voltage at given time by linear interpolation of two
 * nearest values given by input file.
 */
double BoundaryCondition::interpolate(
        QMap<qreal, qreal>::iterator low,
        QMap<qreal, qreal>::iterator high,
        double time)
{
    if (qFabs(high.key() - time) <= 1e-8)
        return high.value();
    if (qFabs(low.key() - time) <= 1e-8)
        return low.value();
    return low.value() + (time - low.key())*
            (high.value() - low.value())/(high.key() - low.key());
}
