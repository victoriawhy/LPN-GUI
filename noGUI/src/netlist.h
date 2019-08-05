/*
Netlist.h
---------
Class to represent a netlist.
*/

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "boundarycondition.h"

#ifndef __NETLIST_H__
#define __NETLIST_H__

class Netlist
{
public:
	Netlist(const std::string &name = "");
	~Netlist();

	/*
	Return char * array containing netlist, which can be passed
	to ngSpice_Circ. Returns NULL on error.
	*/
	char** get_netlist();
	
	/*
	Get boundary condition at given node at the given time. Called by
	ngspice callback.
	*/
	double get_boundary_condition(const std::string &node_name, double time);

	/*
	Load a netlist from a file. Optionally pass a pointer to a dictionary
	specifying external boundary condition files, and a period for these files.
	*/
	int load_from_file(
		const std::string &filename,
		const std::map<std::string, std::string>* bc_files = NULL,
		const double period = 0.0
	);

private:
	std::map<std::string, BoundaryCondition*> bcs;
	std::vector<std::string> netlist_vec;
	char** netlist;
	bool file_loaded;
	bool constructed;

	/* Add a boundary condition for the element given by element_name */
	int add_boundary_condition(
		const std::string &element_name, 
		const std::string &file_given = "", 
		const double &period_given = 0.0
	);
	/* Free the memory associated with the netlist */
	void unload_netlist();
	/* Get the length of the netlist - used for freeing the char* array */
	size_t get_length();
	/* Construct the char* array */
	int construct_netlist();
	/* Request boundary condition info from user. Called if add_boundary_condition() is called without
	a file and period */
	void request_boundary_condition(const std::string &node_name, std::string &file, double &period);
};


#endif

// void set_name(const std::string name);
// int add_element(
// 	const std::string &element, 
// 	const std::string &name, 
// 	const std::string &node_in, 
// 	const std::string &node_out, 
// 	const std::string &value, 
// 	const std::string &units = ""
// );
// int add_initial_condition(const std::string &node_name, const std::string &ic);
// int set_analysis(
// 	const std::string &type, 
// 	const std::string &step, 
// 	const std::string &step_units, 
// 	const std::string &time, 
// 	const std::string &time_units
// );
// bool check_file_loaded();