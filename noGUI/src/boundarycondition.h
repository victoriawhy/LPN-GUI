/*
BoundaryCondition.h
---------
Class to represent the boundary conditions for an element.

A file given to the BoundaryCondition constructor should have each line formatted:
<time> <value>\n
Where time and value are doubles.
The behavior of a BoundaryCondition with an invalid file is undefined.
*/
#include <string>
#include <map>

#ifndef __BOUNDARYCONDITION_H__
#define __BOUNDARYCONDITION_H__

class BoundaryCondition
{
public:
	/* Constructor: read given file into conditions map, and save period */
	BoundaryCondition(const std::string &filename, double period);
	~BoundaryCondition() {}

	/* Return the state at the given time, linearly interpolating between the 
	two closest defined points. */
	double get_state(double time);


private:
	std::map<double, double> conditions;
	double period;

	double interpolate(double t_lower, double t_upper, double c_lower, double c_upper, double t);

};

#endif