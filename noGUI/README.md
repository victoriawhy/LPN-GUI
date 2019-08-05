## Command Line Program for Running a Netlist with NGSPICE

### About the program
This program is intended to allow you to run simulations using Ngspice from the command line without the LPN GUI interface. You can write your own Ngspice script (read the Ngspice manual to learn how to do this), or use the LPN GUI to build a circuit and save your model with simulation commands. You can then copy this file to a cluster machine, or wherever you want to run your simulation, and use this program to run the file.

This program will run the commands listed in the file provided, then offer to save any or all of the resulting vectors in ASCII or compact binary format.

### Prerequisites
This program requires Ngspice installed with the shared library. Download the tarball from [here](https://sourceforge.net/projects/ngspice/files/ng-spice-rework/28/ngspice-28.tar.gz/download) or clone the git repository with the command `git clone git://git.code.sf.net/p/ngspice/ngspice` and follow the instructions in the file `INSTALL` in the top level `ngspice` directory. Use the flag `--with-ngshared` to the `./configure` command.

### To run the program
Type `make` to compile the simulator executable.

Run the program with `./simulator <circuit filename>`. Example files are provided in the `example_circuits` directory. `noext.cir` and `ext.cir` both describe a transient analysis on an RCR model, however in `noext.cir`, the voltage source is constant, and in `ext.cir` the voltage source is external. For `ext.cir`, example source files are provided in the `pressure_samples` directory.

If you want to suppress any requests for input, you can use the flag `-s` or `--silent` to run the simulation and save all vectors
in ASCII format to the file out.raw. CAUTION: out.raw may be overwritten if you run this repeatedly without renaming or moving out.raw. If your netlist contains external input elements, you will still need to provide a filename and period at the prompt.

### Extensions to this code
If you want to customize the functionality, you can either run your simulation using the Ngspice command line interpreter, or customize the program code.

If you want to use Ngspice directly, enter the interpreter by entering the command `ngspice` at a terminal prompt. You can then load your file with the command `source <filename>`, and run the simulation with the command `run`. The commands `write` and `plot` will save and plot your vectors, respectively. `plot` will not work if you don't have X installed. To save vectors in ASCII format, enter the command `set filetype=ascii` before the `write` command. Chapter 17 of the Ngspice manual covers using the interpreter in detail.

If you want to customize this code, edit the file `main.cc`, below the comment that begins "To customize this program, edit the code below."

I would also recommend implementing more command line arguments and flags, to reduce the need for user input (e.g. flags to specify boundary condition files). Another useful extension would be to allow the user to provide a YAML file as a command line argument that contains the file and period for each external element.