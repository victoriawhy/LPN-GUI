import sys
import os 
import numpy as np


PREFIX_VOLTAGE = "V"
PREFIX_RESISTOR = "R"
PREFIX_CAPACITOR = "C"
PREFIX_FLOW = "I"
PREFIX_START = "start"

# Reads in all input from the GUI file and filters out repeated entries
# Stores the name of the inlet and outlet boundaries for further parsing 
# Returns elementID, element_values, boundary and connectivity dictionaries
# to write the appropriate text files 
def read_gui_input(input_file): 
	#Initialize data structures and vars 
	all_elements = []
	elementID = {}
	element_values = {}
	boundary = {}
	connectivity = {}
	inlet_name = " "
	i = 1
	for line in input_file: 
		info = line.split()
		all_elements.append(info)
	# Filter out repeats
	all_elements = [list(x) for x in set(tuple(x) for x in all_elements)]
	# Store inlet name, element types & values, boundary names & values, connectivity 
	for elem_pair in all_elements: 
		# Store inlet name to pass into find_inlet
		if elem_pair[0] == PREFIX_START: 
			inlet_name = elem_pair[2]
			print "Found inlet: ", inlet_name
			continue 
		# Set outlet boundaries 
		elif len(elem_pair) == 2: 
			print "Curr boundary pair: ", elem_pair
			boundary[elem_pair[0]] = (elem_pair[0][0], elem_pair[1])
			print "Put into boundary dict: ", elem_pair[0], "\t", boundary[elem_pair[0]]
			continue 
		# Add elements in LPN 
		else: 
			if elem_pair[0] not in elementID.keys():
				print i
				elementID[elem_pair[0]] = i 
				print "Added: ", elem_pair[0], elementID[elem_pair[0]]
				i += 1
		# Dictionary element values | element name:(prefix, value)
		element_values[elem_pair[0]] = (elem_pair[0][0], elem_pair[1]) 
		element_values[elem_pair[2]] = (elem_pair[2][0], elem_pair[3])
		# Dictionary connectivity | element name:[element name(s)]
		if elem_pair[0] in connectivity.keys(): 
			connectivity[elem_pair[0]].append(elem_pair[2])
		else: 
			connectivity[elem_pair[0]] = [elem_pair[2]]
	# Remove outlet boundaries from element_values, connectivity dictionaries 
	# Append to boundary 
	element_values, connectivity, boundary = \
	find_boundaries(elementID, element_values, connectivity, boundary)
	# Remove inlet boundary from element_values, connectivity dictionaries
	# Append to boundary 
	elementID, element_values, connectivity, boundary = \
	find_inlet(inlet_name, elementID, element_values, connectivity, boundary)
	#Return updated dictionaries 
	return elementID, element_values, connectivity, boundary

# Takes in elementID, element_values, connectivity and boundary dictionaries 
# Removes boundary elements from element_values and connectivity dictionaries
# Updates boundary dictionary so that the key is the element connected to the 
# BC, and the value is tuple(prefix, value) of the BC
# i.e. if R3 is connected to an outlet resistance with value 5, 
# then boundary has the entry R3:(R, 5)
# Returns updated element_values, connectivity, boundary 
def find_boundaries(elementID, element_values, connectivity, boundary): 
	boundary_elems = []
	to_remove = []
	for key, value in connectivity.iteritems(): 
		#If an elem is connected to a boundary, change the element key in boundary
		BC = list(set(value) & set(boundary.keys())) # checks if the key is connected to a boundary element 
		if BC: 
			BC = BC[0]
			boundary_elems.append((key, BC)) 
			# boundary[key] = element_values[BC]
			# boundary.pop(BC, None)
	for b in boundary_elems: 
		# b[0] = element connected to BC; b[1] = BC itself
		boundary[b[0]] = element_values[b[1]]
		boundary.pop(b[1], None)
		connectivity[b[0]].remove(b[1])
		to_remove.append(b[1])
		# element_values.pop(b[1], None)
	# to handle RCR elements:
	# remove from element values after updating boundary
	# by comparing the keys in boundary 
	# to the keys in element_values
	for r in to_remove: 
		element_values.pop(r, None)

	
	print "New Elem Vals: ", element_values
	print "New cnx: ", connectivity
	print "New bdrs: ", boundary
	return element_values, connectivity, boundary

# Takes in inlet_name, elementID, element_values, connectivity, and boundary 
# Removes inlet from the element_values and connectivity dictionaries 
# Updates the boundary dictionary so that the inlet entry is 
# inlet name:(prefix, value)
# Reads in the value of the inlet as either a list of nonconstant input
# or as a single value for constant input 
# Returns updated elementID, element_values, connectivity and boundary 
def find_inlet(inlet_name, elementID, element_values, connectivity, boundary): 
	if inlet_name[-3:] == "_dt": 
		input_filename = element_values[inlet_name][1]
		input_file = open(input_filename, 'r')
		input_dt = read_dt_file(input_file)
		boundary[inlet_name] = (inlet_name[0], input_dt)
	else: 
		boundary[inlet_name] = (inlet_name[0], element_values[inlet_name][1])
	for e in connectivity[inlet_name]: 
		print "Parsing inlet connectivities: ", e, elementID[e] 
		elementID[e] = 0 
		print "New ID: ", e, elementID[e]
		connectivity.pop(inlet_name, None)
		element_values.pop(inlet_name, None)
	elementID[inlet_name] = 0
	print "Element IDs: ", elementID, "\n"
	print "connectivity: ", connectivity, "\n"
	return elementID, element_values, connectivity, boundary


# Read in nonconstant input 
# Returns array of strings such that 
# each pair of values is time, value 
def read_dt_file(input_file):
	flow = [] 
	for line in input_file: 
		info = map(str, line.split())
		flow.append(info)
	return flow 

# Converts all element names to correponding ID numbers 
def addElemID(elementID, connectivity, boundary, element_values): 
	bifurcations = {}
	boundary_conditions = {}
	elem_type_val = {}
	# Bifurcations 
	for key, value in connectivity.iteritems(): 
		print key, value
		key_ID = elementID[key]
		val_ID = [elementID[v] for v in value]
		bifurcations[key_ID] = val_ID
	# Boundary Conditions 
	for key, value in boundary.iteritems(): 
		key_ID = elementID[key]
		if key[0] == PREFIX_RESISTOR: 
			bifurcations[key_ID] = " "
		boundary_conditions[key_ID] = value[0], value[1] 
	# Element value types
	for key, value in element_values.iteritems(): 
		key_ID = elementID[key]
		elem_type_val[key_ID] = (value[0], value[1])
	# Remove overlap where key:value and value:key 
	for key in bifurcations.keys(): 
		if len(bifurcations[key]) > 1:   
			for v in bifurcations[key]:
				if key in list(bifurcations[v]): 
					bifurcations[key].remove(v)
					bifurcations[v].remove(key)
	return bifurcations, boundary_conditions, elem_type_val

# Writes all elements, its type and its value to an outfile  
def write_element_file(file_handle, element_values): 
	with open(file_handle, "w") as outfile: 
		for key, value in element_values.iteritems(): 
			if type(value[1]) != list:
				outfile.write(str(key) + ": " + value[0] + ": " + value[1] + "\n")
			else: 
				outfile.write(str(key) + ": " + value[0] + ": ")
				value_str  = [item for sublist in value[1] for item in sublist]
				for v in value_str: 
					outfile.write(v + " ")
				outfile.write("\n")

# Writes all bifurcations/connectivities to an outfile
def write_bifurcations(file_handle, bifurcations):
	with open(file_handle, "w") as outfile: 
		for key, value in bifurcations.iteritems(): 
			value_str = " ".join(map(str, value))
			outfile.write(str(key) + ": " + value_str + "\n")

# Writes all boundary conditions to an outfile 
def write_boundary(file_handle, boundary): 
	print "At write_boundary\n"
	with open(file_handle, "w") as outfile: 
		for key, value in boundary.iteritems():
			print "Curr key: ", key, "\n" 
			# Resistance boundary conditions 
			if value[0] == PREFIX_RESISTOR: 
				# print key, ":OUTLET RESISTANCE 0.0 " + value[1] + "\n"
				outfile.write(str(key) + ":OUTLET RESISTANCE 0.0 " + value[1] + "\n")
				continue
			# Flow/pressure inlet conditions 
			if value[0] == PREFIX_FLOW:
				outfile.write("0:INLET FLOW ")
			elif value[0] == PREFIX_VOLTAGE: 
				outfile.write("0:PRESSURE SOURCE ") 
			if type(value[1]) != list: 
				outfile.write("0.0 " + str(value[1]) + " 1.0 " + str(value[1]) + "\n")
			else: 
				print "Variable value: ", key
				flow_string = [item for sublist in value[1] for item in sublist]
				for f in flow_string: 
					outfile.write(f + " ")
				outfile.write("\n")
 
def main(): 
	input_file = open(sys.argv[1], 'r')
	elementID, element_values, connectivity, boundary = read_gui_input(input_file)
	bifurcations, boundary_conditions, elem_type_val = \
	addElemID(elementID, connectivity, boundary, element_values)
	element_handle = "elementTypesAndValues.txt"
	write_element_file(element_handle, elem_type_val)
	bifur_handle = "bifurcations.txt"
	write_bifurcations(bifur_handle, bifurcations)
	boundary_handle = "boundaryConditions.txt"
	write_boundary(boundary_handle, boundary_conditions)
	# os.system('python /Users/vyuan/Marsden-Lab/0D_LPN_Python_Solver-master/GUI_input_test/test_0D_solver.py')
	import test_0D_solver

if __name__=="__main__":
	main()


'''
	# Populate dictionaries 
	for elem_pair in all_elements: 
		# Find start ground and set element ID to 0 for inlet 
		if elem_pair[0] == PREFIX_START: 
			inlet_name = elem_pair[2]
			continue
			#elementID[elem_pair[2]] = 0
		# 	if elem_pair[2] in element_values.keys():  
		# 		boundary[elem_pair[2]] = (elem_pair[2][0], element_values[elem_pair[2]][1])
		# 	continue 
		# # Nonconstant input 
		# if elem_pair[0] == inlet_name:
		# 	if elem_pair[0][-3:] == "_dt":
		# 		input_file = open(elem_pair[1], 'r')
		# 		input_dt = read_dt_file(input_file)
		# 		boundary[elem_pair[0]] = (elem_pair[0][0], input_dt)
		# 		#element_values[elem_pair[0]] = (elem_pair[0][0], input_dt)
		# 	# Constant input  
		# 	else: 
		# 		boundary[elem_pair[0]] = (elem_pair[0][0], elem_pair[1])
		# Elements connected to ground [aka boundary conditions]
		elif len(elem_pair) == 2: 
			boundary[elem_pair[0]] = (elem_pair[0][0], elem_pair[1])
			element_values[elem_pair[0]] = (elem_pair[0][0], elem_pair[1])
			if elem_pair[0] not in elementID.keys(): 
				elementID[elem_pair[0]] = i 
				i += 1
			continue
		else: 
			if elem_pair[0] not in elementID.keys():
				elementID[elem_pair[0]] = i 
				i += 1
			if elem_pair[2] not in elementID.keys(): 
				elementID[elem_pair[2]] = i 
				i += 1

		#Store element:value pairs 
		element_values[elem_pair[0]] = (elem_pair[0][0], elem_pair[1])
		element_values[elem_pair[2]] = (elem_pair[2][0], elem_pair[3])

		#Store element connectivity 
		if elem_pair[0] in connectivity.keys(): 
			connectivity[elem_pair[0]].append(elem_pair[2])
		else: 
			connectivity[elem_pair[0]] = [elem_pair[2]]
'''



