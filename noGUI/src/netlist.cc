/*
netlist.cc
----------
Implement netlist class
*/

#include <fstream>
#include <string.h>
#include "netlist.h"

Netlist::Netlist(const std::string &name)
{
	this->netlist = NULL;
	this->file_loaded = false;
	this->constructed = false;
}

Netlist::~Netlist()
{
	for (auto const& val : bcs)
		delete val.second;
	
	if(!this->netlist) return;
	
	this->unload_netlist();
}

int Netlist::load_from_file(
	const std::string &filename, 
	const std::map<std::string,std::string>* bc_files,
	const double period
)
{
	this->unload_netlist();
	// open file
	std::ifstream f(filename);
    char line[256];
    while(true) {
        // read file
        if(f.eof()) break;
        f.getline(line, 256);
        std::string l = std::string(line);

        // check for boundary conditions necessary
        if (l.find("external") != std::string::npos) {
        	std::string elem_name = l.substr(0, l.find(" "));
        	
        	if (bc_files && bc_files->find(elem_name) != bc_files->end()) {
        		this->add_boundary_condition(elem_name, bc_files->at(elem_name), period);
        	} else {
        		this->add_boundary_condition(elem_name);
        	}
        }

        // add to netlist
        if (l.find(".end") == std::string::npos)
	        this->netlist_vec.push_back(l);
    }

    this->file_loaded = true;
	return 0;
}

int Netlist::add_boundary_condition(
	const std::string &element_name,
	const std::string &file_given, 
	const double &period_given
)
{	

	std::string file;
	double period;
	if (file_given == ""){
		this->request_boundary_condition(element_name, file, period);
	} else {
		file = file_given;
		period = period_given;
	}

	BoundaryCondition *cond = new BoundaryCondition(file, period);
	char node_name_lower[element_name.length()];
	for (size_t i = 0; i < element_name.length(); i++) {
		node_name_lower[i] = std::tolower(element_name[i]);
	}
	this->bcs[node_name_lower] = cond;
	return 0;
}

void Netlist::request_boundary_condition(
	const std::string &node_name, 
	std::string &file, 
	double &period
) 
{
	std::cout << "Provide file containing boundary conditions for " << node_name << ": ";
	std::getline(std::cin, file);
	std::cout << "Provide period for cardiac cycle: ";
	std::string period_str;
	std::getline(std::cin, period_str);
	period = std::stod(period_str);
}


double Netlist::get_boundary_condition(const std::string &node_name, double time)
{
 	BoundaryCondition *bc = bcs[node_name];
 	return bc->get_state(time);
}

char** Netlist::get_netlist()
{	
	if (this->construct_netlist() == 1)
		return NULL;
	return this->netlist;
}

int Netlist::construct_netlist()
{	
	if (this->netlist != NULL) {
		std::cout << "Error: unload netlist before constructing a new one." << std::endl;
		return 1;
	}

	this->netlist = new char*[this->netlist_vec.size() + 2];

	size_t i;
	for (i = 0; i < this->netlist_vec.size(); i++) {
		this->netlist[i] = strdup(this->netlist_vec[i].c_str());
	}

	this->netlist[i] = strdup(".end");
	this->netlist[i + 1] = NULL;
	this->constructed = true;
	return 0;
}

size_t Netlist::get_length()
{
	if (!this->constructed) return -1;
	return this->netlist_vec.size() + 1;
}

void Netlist::unload_netlist()
{
	if(this->netlist != NULL) {
		for (size_t i = 0; i < this->get_length(); i++)
			delete this->netlist[i];
	}

	delete this->netlist;

	this->netlist = NULL;
	this->netlist_vec.clear();

	this->file_loaded = false;
	this->constructed = false;
}
	

// UNUSED FUNCTIONS
// The functions below may help if you want to extend this code to allow for building
// netlists within the program.

// bool Netlist::check_file_loaded()
// {
// 	if(this->file_loaded){
// 		std::cout << "Error: Cannot edit netlists loaded from file." << std::endl;
// 		return true;
// 	}
// 	return false;
// }

// void Netlist::set_name(const std::string name)
// {
// 	if (this->check_file_loaded()) return;
// 	if (this->netlist_vec.empty()) {
// 		this->netlist_vec.push_back(name);
// 	} else {
// 		this->netlist_vec[0] = name;
// 	}
// }

// int Netlist::add_element(
// 	const std::string &element, 
// 	const std::string &name, 
// 	const std::string &node_in, 
// 	const std::string &node_out, 
// 	const std::string &value, 
// 	const std::string &units
// )
// {	
// 	if (this->check_file_loaded()) return 1;

// 	// construct element
// 	std::string element_line = element + name + " "  + node_in + " " + node_out + " " + value;
// 	if(units.length() != 0) element_line += units;
	
// 	// add boundary conditions if necessary
// 	if (value.compare("external") == 0) { // TODO : User tolower!
// 		this->add_boundary_condition(element + name);
// 	}

// 	this->netlist_vec.push_back(element_line);

// 	return 0;
// }

// int Netlist::add_initial_condition(const std::string &node_name, const std::string &ic)
// {	
// 	if (this->check_file_loaded()) return 1;
// 	std::string name;
// 	size_t idx;
// 	std::stoi(node_name, &idx);
// 	name = (node_name.substr(idx) == "" ? "v(" + node_name + ")" : node_name);

// 	this->netlist_vec.push_back(".ic " + name + "=" + ic);
// 	return 0;
// }

// // TODO: Make this flexible for different analysis lines (.tran .dc)
// int Netlist::set_analysis(
// 	const std::string &type, 
// 	const std::string &step, 
// 	const std::string &step_units, 
// 	const std::string &time,
// 	const std::string &time_units
// )
// {
// 	if (this->check_file_loaded()) return 1;

// 	std::string analysis = type + " " + step + step_units + " " + time + time_units;

// 	this->netlist_vec.push_back(analysis);
// 	return 0;
// }