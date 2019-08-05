#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include <QtWidgets>
#include <cmath>
#include <QtMath>

/* CLASS: BoundaryCondition
 * ========================
 * Inherits: QObject
 * Parent: ?
 *
 * A BoundaryCondition object parses an input file of the format <time> <value>
 * and allows the user to get the value at the given boundary at any given
 * time.
 *
 * The value is linearly interpolated between points in the given file.
 *
 * The static funciton checkFile(QString filename) allows other classes to check
 * if a file is properly formatted to be used as a BoundaryCondition file.
 */
class BoundaryCondition : public QObject
{
    Q_OBJECT
public:
    explicit BoundaryCondition(QString filename,
                               QObject *parent = nullptr);
    double getState(double time);
    static bool checkFile(QString filename);

private:
    QMap<qreal, qreal> states;
    double period;
    double interpolate(
            QMap<qreal, qreal>::iterator low,
            QMap<qreal, qreal>::iterator high,
            double time);


signals:
    void badFile();

public slots:
};

#endif // BOUNDARYCONDITION_H
