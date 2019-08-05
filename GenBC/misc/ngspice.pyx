# Create a python library using Cython to interface
# with ngspice shared lib. Unimplemented.
# Generate library with 
# > python setup.py
# and then import in a python script with
# > import ngspice
cimport sharedspice

cdef class NgSpice:
	cdef sharedspice.vecvalues vv
	def __cinit__(self):
		self.vv.creal = 1.2
		print(self.vv.creal)
