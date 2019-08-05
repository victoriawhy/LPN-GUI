
.SUFFIXES: .f .o .pyx .c

ENVDIR = /home/srobertson/env
LIBDIR = /usr/local/lib

FORTRAN = gfortran
FOPTION = -O3 -g -p -pg
FFLAGS += -g -Wall -fimplicit-none -fbounds-check -p -fbacktrace -ftrapv -Wline-truncation -fcheck=all #-pedantic
MYFUN   = Modules.o USER.o GenBC.o
MYFUN2  = test.o
CC = gcc
CYTHON = cython --embed
MYFUN_CY = GenBC_cy.c 
CFLAGS = -I$(ENVDIR)/cython/include/python3.5m -I$(LIBDIR)/ngspice
LDFLAGS = -lpython3.5m -lngspice


all: GenBC test GenBC_cy

GenBC:	$(MYFUN)
	$(FORTRAN) $(MYFUN) $(FFLAGS) -o $@

test:	$(MYFUN2)
	$(FORTRAN) $(MYFUN2) $(FOPTION) -o $@

GenBC_cy: $(MYFUN_CY)
	$(CC) $(CFLAGS) $(MYFUN_CY) -o $@ $(LDFLAGS)

.pyx.c: 
	$(CYTHON) $< 

.f.o:	
	$(FORTRAN) $(FFLAGS) -c $< $(VERBOSE)

clean:	
	rm -f *.o *.mod *.out *.c *~ test GenBC GenBC_cy InitialData GenBC.int QGeneral

