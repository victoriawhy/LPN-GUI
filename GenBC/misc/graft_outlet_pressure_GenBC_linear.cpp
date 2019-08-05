// ====================================================================================
// GenBC for Neumann condition inflow/outflow. This function will read in the
//             Fourier coefficients for the pressure wave and output the binary
//             files that SimVascular will read to apply the Neummann BC               
// Justin Tran, 2017
// ====================================================================================

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>

#include "lpnUtils.hpp"
#include "cmException.h"

// Set the name of the file that will contain the Fourier coefficients
std::string coefficientsFile = "outlet_pressures.dat";

// Std vectors to store the Fourier coefficients that we will read from files
double period;
std::vector<double> times;
std::vector<double> pressures;
int numFaces = 1; // Set to 1 for now, can extend to multiple faces later

const double pConv = 1E0;
const double qConv = 1E0;

// Read the pressure data
void readPressures(std::string filename)
{
  std::ifstream read_file;
  read_file.open(filename.c_str());
  std::string buffer;
  std::vector<std::string> tokens;
  
  if(!read_file.is_open())
  {
    throw cmException("ERROR: Cannot open pressure file!");
  }
  
  // Skip header line
  std::getline(read_file, buffer);
  
  while(!read_file.eof())
  {
    std::getline(read_file, buffer);
    schSplit(buffer, tokens, ",");
    if(tokens.size() != 2)
    {
      break;
    }
    times.push_back(atof(tokens[0].c_str()));
    pressures.push_back(atof(tokens[1].c_str()));
  }
  
  period = times[times.size()-1];
  read_file.close();
}


// Linear interpolation of pressure
double outputSignal(double tc)
{
  // Need to find the time in cycle
  double tInCycle = fmod(tc,period);
  int lower_ind = 0;
  for(int i = 1; i < times.size(); i++)
  {
    if(times[i] > tInCycle)
    {
      lower_ind = i-1;
      break;
    }
  }
  
  double t_width = times[lower_ind+1] - times[lower_ind];
  double weight1 = (tInCycle - times[lower_ind]) / t_width;
  double weight2 = (times[lower_ind+1] - tInCycle) / t_width;
  
  double output = weight1*pressures[lower_ind+1] + weight2*pressures[lower_ind];
  
  return output;
}

// =======================
// READ PHASTA GENBC FILES
// =======================
void readSimFiles(double &currTime, int &nDirichletSurfaces, int &nNeumannSurfaces, double &timeStep, double* Pi, double* Pf, double* Qi, double* Qf, double &p_last, char &flag) 
{
   int int_size = sizeof(int);
   int char_size = sizeof(char);
   int double_size = sizeof(double);
   int size_buffer;

   ifstream genBC_reader;
   genBC_reader.open("GenBC.int", ios::in|ios::binary);
   if(genBC_reader.is_open()) 
   {

      // Read in flag, timestep size, number of Dirichlet surfaces, and
      // number of Neumann surfaces from SimVascular 3D solver
      genBC_reader.read( (char*)&size_buffer, int_size );
      genBC_reader.read( (char*)&flag, size_buffer );
      genBC_reader.read( (char*)&size_buffer, int_size );

      genBC_reader.read( (char*)&size_buffer, int_size );
      genBC_reader.read( (char*)&timeStep, size_buffer );
      genBC_reader.read( (char*)&size_buffer, int_size );

      genBC_reader.read( (char*)&size_buffer, int_size );
      genBC_reader.read( (char*)&nDirichletSurfaces, size_buffer );
      genBC_reader.read( (char*)&size_buffer, int_size );

      genBC_reader.read( (char*)&size_buffer, int_size );
      genBC_reader.read( (char*)&nNeumannSurfaces, size_buffer );
      genBC_reader.read( (char*)&size_buffer, int_size );

      // Read in pressures from the Dirichlet surfaces
      for(int i = 0; i < nDirichletSurfaces; i++) 
      {
         genBC_reader.read( (char*)&size_buffer, int_size );
         genBC_reader.read( (char*)&Pi[i], double_size );
         genBC_reader.read( (char*)&Pf[i], double_size );
         genBC_reader.read( (char*)&size_buffer, int_size );

         Pi[i] = Pi[i]/pConv;
         Pf[i] = Pf[i]/pConv;
      }

      // Read in flows from Neumann surfaces
      for(int i = 0; i < nNeumannSurfaces; i++) {
         genBC_reader.read( (char*)&size_buffer, int_size );
         genBC_reader.read( (char*)&Qi[i], double_size );
         genBC_reader.read( (char*)&Qf[i], double_size );
         genBC_reader.read( (char*)&size_buffer, int_size );
      }

      genBC_reader.close();
   }
   else {
      throw cmException("Error: Could not find GenBC.int from PHASTA.\n");
   }


   ifstream init_reader;
   init_reader.open("InitialData", ios::in|ios::binary);
   if(init_reader.is_open()) {

      init_reader.read( (char*)&size_buffer, int_size );
      init_reader.read( (char*)&currTime, size_buffer );
      init_reader.read( (char*)&size_buffer, int_size );

      init_reader.read( (char*)&size_buffer, int_size );
      init_reader.read( (char*)&p_last, size_buffer );
      init_reader.read( (char*)&size_buffer, int_size );

   }
   else {
      // Then we are initializing the simulation and use the pre-defined
      // initial conditions in PerformRK4Steps. We do nothing in this
      // function if this file is not found since it is generated at the
      // end of PerformRK4Steps if we are running together with a 3D
      // simulation
   }

}

// ========================
// WRITE PHASTA GENBC FILES
//=========================
void writeSimFiles(double &currTime, double P_out, char flag) {

   int int_size = sizeof(int);
   int char_size = sizeof(char);
   int double_size = sizeof(double);
   double state_buffer;

   // Write the GenBC results from this timestep to communicate with PHASTA
   ofstream genBC_writer;
   genBC_writer.open("GenBC.int", ios::out|ios::binary);

   genBC_writer.write( (char*)&double_size, int_size);
   genBC_writer.write( (char*)&P_out, double_size);
   genBC_writer.write( (char*)&double_size, int_size);

   genBC_writer.close();

   // Write out state variables for use by GenBC in next iteration
   // ONLY update InitialData if on the LAST iteration (i.e. flag == 'L')
   if(flag == 'L') {
      ofstream init_writer;
      init_writer.open("InitialData", ios::out|ios::binary);

      init_writer.write( (char*)&double_size, int_size);
      init_writer.write( (char*)&currTime, double_size);
      init_writer.write( (char*)&double_size, int_size);

      init_writer.write( (char*)&double_size, int_size);
      init_writer.write( (char*)&P_out, double_size);
      init_writer.write( (char*)&double_size, int_size);

      init_writer.close();
   

      // Write to a running AllData file 
      FILE* allData_writer;
      allData_writer = fopen("AllData","a");

      fprintf(allData_writer,"%.6f ",P_out);
      
      fprintf(allData_writer,"\n");

      fclose(allData_writer);
   }
}

int main(int argc, char* argv[])
{
  double currTime = 0.0;
  int nDirichletSurfaces;
  int nNeumannSurfaces;
  double timeStep;
  double Pi[numFaces];
  double Pf[numFaces];
  double Qi[numFaces];
  double Qf[numFaces];
  char flag;
  
  double P_out;
  
  // Read in Fourier coefficients and time period
  readPressures(coefficientsFile);
  
  // Read in 3D solver information
  readSimFiles(currTime, nDirichletSurfaces, nNeumannSurfaces, timeStep, Pi, Pf, Qi, Qf, P_out, flag);
  
  // Compute the pressure at the outlet face
  P_out = outputSignal(currTime);
  
  currTime += timeStep;
  // Write the pressure back to the 3D solver
  writeSimFiles(currTime, P_out, flag);

  return 0;
}


























































