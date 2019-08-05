# GenBC
This directory contains code I've written to start developing a new GenBC system. The Python packages required are in `requirements.txt` and can be install with the command `pip install -r requirements.txt`. I recommend using a virtual environment.
## Contents
* `testing`: contains code I'm currently working on to replace GenBC with a cython executable.
* `fortran_bin`: contains a small test fortran binary file and some C++ code to read it. This was used to understand how to use `scipy.io.FortranFile` and `numpy.fromfile` to read Fortran binary. The Python code is not included.
* `misc`: contains miscellaneous testing code I used to learn more about GenBC and design the new system. This includes some starter code to develop a Python wrapper library to the Ngspice library that would allow Ngspice simulations to be run in pure Python. There also exists a package `PySpice` that implements this behavior.

## `testing`
The goal is to replicate the behavior of `GenBC.f` in `GenBC_cy.pyx`. The behavior is tested by `test.f` (read the comments at the beginning of that file for more info). `test.f` calls an executable, which you can specify as `./GenBC` or `./GenBC_cy`.

The first time you run this code:
* Once you've set up your virtual environment, edit the `Makefile` so that `ENVDIR` contains the path to your environment directory. While you're at it, make sure that `/usr/local/lib/ngspice` exists, and if not edit `LIBDIR` to contain the path to the Ngspice library.

To run the test program:
* Open `test.f` and edit the argument to the function `CALL system()` to  `./GenBC` or `./GenBC_cy`, as desired.
* Run `make` to compile the executables `test`, `GenBC` and `GenBC_cy`.
* Run the test with `./test`

The file `AllData_template` contains the "solution" output that we want to create from `GenBC_cy`.