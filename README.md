# LPN Plugin
## About
This repository contains all development code for a GUI plugin for lumped parameter modeling in SimVascular. The ultimate goal is to provide an intuitive and extensible graphical interface for designing and simulating lumped parameter networks, for use in both independent simulations and as coupled boundary conditions to 1-D and 3-D models. The current design allows users to drag-and-drop circuit elements to build a model, and provides a step-by-step wizard to run independent simulations. All simulations are run by Ngspice using their shared C library.
## In This Repo
### simulator
The `simulator` directory contains the Qt project for the standalone GUI for building and simulating independent LPNs. It is envisioned that this will be the basis for the eventual SimVascular plugin.

### noGUI
The `noGUI` directory contains code to provide similar simulation functionality to the GUI wizard, but without the GUI interface. This may be helpful for running simulations on a cluster machine. See the README in the `noGUI` directory for more information.

### GenBC
The `GenBC` directory contains code used for testing and development of a coupled boundary conditions system using Ngspice, to replace the current `GenBC` system. This code is currently not ready for use.

## Getting Started
### Dependencies
#### Ngspice Shared Library
All simulation functionality across all parts of this project is handled by the [Ngspice](http://ngspice.sourceforge.net) shared library. In order to run the simulator you will need to download and install Ngspice and the shared library.

1. Clone the ngspice repo: `git clone http://git.code.sf.net/p/ngspice/ngspice` or download the tarball from [this link](https://sourceforge.net/projects/ngspice/files/ng-spice-rework/28/ngspice-28.tar.gz/download)
2. Follow the install instructions in the file `INSTALL` in the top `ngspice` directory, but ensure to include the flag `--with-ngshared` to the `./configure` command. (i.e. you should run this command: `./configure --enable-xspice --enable-cider --disable-debug=yes --with-ngshared && make && sudo make install`).
4. Make sure that the file `/usr/local/include/ngspice/sharedspice.h` exists

#### Qt 5.11 and Qt Creator
Download and install Qt Creator and Qt 5.11 from [the Qt website](https://www.qt.io/download). The free open source version is fine.

#### Gnuplot
For plotting functionality, you need to install Gnuplot. On Linux you can download it with the command `apt-get install gnuplot`.

### Build and run
To run the circuit simulator, open Qt Creator and click Open Project. Open the file `simulator/simulator.pro`. Qt Creator should automatically configure the project for you. You can build and run using the Qt Creator buttons in the lower left corner.

### Using the GUI
Check out the tutorial [here](https://docs.google.com/document/d/1J-ggab6n2It53UR8Kjt0JH-hVUsA8RUDbks9r8pQSlk/edit?usp=sharing) to learn how to use the program.

## For Developers

### Known Bugs / Issues
* There is no way to delete a connection between elements without deleting one of the elements. Wires should be selectable and deletable
* The way wires are drawn right now may not be ideal for further development (one segment of each wire is drawn by each node in the connection, the way the segments are assigned isn't the most aesthetic and isn't dynamic)
* The model parsing algorithm hasn't been rigorously tested
* Very occasionally, an element will jump to the top left corner of the window when clicked and dragged
* The Ngspice callback `ControlledExit` ignores the bool arguments given because the documentation is very confusing

### TODO / Vision
###### For the `simulator` GUI
* Implement clinical units and conversion between these units and ngspice-compatible units
* Allow for time-varying voltage and current sources that aren't external (documented in Chapter 4 of the Ngspice manual)
* The current method of using a 'START' ground node for parsing start isn't optimal or intuitive. Maybe something like a visual representation of an inlet/outlet would be better, or maybe the Voltage/Current sources could only have one wire leading out of them to represent an inlet/outlet.
* Add drag-and-drop blocks e.g. Windkessel or Heart models
* Test and improve support for other operating systems (currently only tested on Linux)
* Implement as a plugin

###### For the `GenBC` system
The vision for the `GenBC` system is that users will be able to open the GUI to build a model for a particular surface (or surfaces, in the case of closed loop boundary condition), assign the model to the surface(s), and be able to run the simulation with no more interaction with the models.
My current proposal is to use Cython to generate an executable that can simply replace the current GenBC executable, requiring no changes to SimVascular. The required functionality of this executable is to:
1. Read the Fortran binary `GenBC.int` to get values at surfaces from SimVascular
2. Run a simulation on each LPN, using these values
3. Write the simulation results back to `GenBC.int` for SimVascular to read

The code in the `GenBC` directory is currently in development to perform these three steps. What is currently working:
1. The `scipy.io.FortranFile` package can read and write to `GenBC.int`
2. The `cython` package is capable of loading the Ngspice shared library and it is callable from Python code.

There are two main challenges currently:
1. How to communicate between the SV GUI plugin and the cython executable regarding:
    * The order in which values need to be read from and written to `GenBC.int` (i.e. the order of the surfaces/models)
    * How to know which value to set in the Ngspice netlist to the value read from `GenBC.int`
    * How to know which vector generated by Ngspice to write to `GenBC.int`
2. Testing and justifying Ngspice simulation
    * Is the Ngspice simulator fast enough to make it worthwhile? It removes the user's need to write out ODEs, but if it increases simulation time significantly this may not be worthwhile
    * What analysis mode actually produces the right values? I've been running transient analysis where the duration is the timestep SV writes to `GenBC.int` and the analysis timestep is the duration divided by `nTimesteps` which is defined as 100 in `USER.f`. So far, I have not been able to replicate the values generated by `GenBC` for the simple test case provided (see directory level `README`).