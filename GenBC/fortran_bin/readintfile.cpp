// Test file to figure out format of GenBC.int
// not meant to be used for anything!

#include <iostream>
#include <ios>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	int size_buffer;
	double val;
    ifstream genBC_reader;
    genBC_reader.open("GenBC.int", ios::in|ios::binary);
    if(genBC_reader.is_open()) 
    {
    	genBC_reader.seekg (0, genBC_reader.end);
	    int length = genBC_reader.tellg();
	    genBC_reader.seekg (0, genBC_reader.beg);
	    cout << "Length of file is " << length << endl;
	    cout << "sizeof(int): " << sizeof(int) << endl;
	    cout << "sizeof(double): " << sizeof(double) << endl;
		// Read in flag, timestep size, number of Dirichlet surfaces, and
		// number of Neumann surfaces from SimVascular 3D solver
		genBC_reader.read( (char*)&size_buffer, sizeof(int) );
		cout << "Size buffer holds " << size_buffer << endl;
		genBC_reader.read( (char*)&val, size_buffer);
		cout << "Val holds " << val << endl;
		if (genBC_reader.eof()) return 0;
		genBC_reader.read( (char*)&size_buffer, sizeof(int) );
		cout << "Size buffer holds " << size_buffer << endl;
		if (genBC_reader.eof()) {
			cout << "EOF" << endl;
		}
		genBC_reader.read( (char*)&size_buffer, sizeof(int) );
		cout << "Size buffer holds " << size_buffer << endl;
		genBC_reader.read( (char*)&val, size_buffer);
		cout << "Val holds " << val << endl;
		if (genBC_reader.eof()) return 0;
		genBC_reader.read( (char*)&size_buffer, sizeof(int) );
		cout << "Size buffer holds " << size_buffer << endl;
		if (genBC_reader.eof()) {
			cout << "EOF" << endl;
		}
		genBC_reader.read( (char*)&size_buffer, sizeof(int) );
		cout << "Size buffer holds " << size_buffer << endl;
		genBC_reader.read( (char*)&val, size_buffer);
		cout << "Val holds " << val << endl;
		if (genBC_reader.eof()) return 0;
		genBC_reader.read( (char*)&size_buffer, sizeof(int) );
		cout << "Size buffer holds " << size_buffer << endl;
    } else {
    	cout << "Failed to open file" << endl;
    }
    return 0;
}