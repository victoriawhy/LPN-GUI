/*
boundarycondition.cc
--------------------
Implement Boundary Condition class
*/

#include <iostream>
#include <fstream>
#include <cmath>

#include "boundarycondition.h"

BoundaryCondition::BoundaryCondition(const std::string &filename, double period) 
{
	this->period = period;

	// TODO: Error checking

	std::ifstream f(filename);
    char line[256];
    while(true) {
        // read file
        f.getline(line, 256);
        if(f.eof()) break;

        // parse time and pressure
        std::string::size_type sz;
        double t = stod(std::string(line), &sz);
        double p = stod(std::string(line).substr(sz));

        // add to boundary conditions map
        this->conditions[t] = p;
    }

}

double BoundaryCondition::get_state(double time) 
{
	std::map<double, double>::iterator high, low;

    time = std::fmod(time, this->period);
    high = this->conditions.lower_bound(time);
    low = (high == this->conditions.begin() ? high : high--);

    return this->interpolate(low->first, high->first, low->second, high->second, time);
}

/* Function to compute interpolated boundary condition */
double BoundaryCondition::interpolate(
	double t_lower, 
	double t_upper, 
	double c_lower, 
	double c_upper,
	double t
) 
{
	if (std::abs(t_upper - t) <= 1e-8)
        return c_upper;

    if (std::abs(t_lower - t) <= 1e-8)
        return c_lower;

	return c_lower + (t - t_lower)*(c_upper - c_lower)/(t_upper - t_lower);
}