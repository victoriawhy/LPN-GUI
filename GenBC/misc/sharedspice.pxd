# Use with cython to create a python library for ngspice
# See ngspice.pyx and setup.py, and the Cython documentation
# for more info

cdef extern from "../sharedspice.h":

    ctypedef struct ngcomplex_t:
        double cx_real,
        double cx_imag

    struct vector_info:
        char *v_name;       # Same as so_vname
        int v_type;         # Same as so_vtype
        short v_flags;      # Flags (a combination of VF_*)
        double *v_realdata;     # Real data
        ngcomplex_t *v_compdata;    # Complex data
        int v_length;       # Length of the vector
    ctypedef vector_info* pvector_info

    struct vecvalues:
        char* name; #name of a specific vector
        double creal; #actual data value
        double cimag; #actual data value
        int is_scale;#if 'name' is the scale vector
        int is_complex;#if the data are complex numbers
    ctypedef vecvalues* pvecvalues

    struct vecvaluesall:
        int veccount; #number of vectors in plot
        int vecindex; #index of actual set of vectors. i.e. the number of accepted data points
        pvecvalues *vecsa; #values of actual set of vectors, indexed from 0 to veccount - 1
    ctypedef vecvaluesall* pvecvaluesall

    #info for a specific vector
    struct vecinfo:
        int number;     #number of vector, as postion in the linked list of vectors, starts with 0
        char *vecname;  #name of the actual vector
        int is_real;   #TRUE if the actual vector has real data
        void *pdvec;    #a void pointer to struct dvec *d, the actual vector
        void *pdvecscale; #a void pointer to struct dvec *ds, the scale vector
    ctypedef vecinfo* pvecinfo

    #info for the current plot
    struct vecinfoall:
        #the plot
        char *name;
        char *title;
        char *date;
        char *type;
        int veccount;

        #the data as an array of vecinfo with length equal to the number of vectors in the plot
        pvecinfo *vecs;
    ctypedef vecinfoall* pvecinfoall

    ctypedef int (*SendChar)(char*, int, void*);
    ctypedef int (*SendStat)(char*, int, void*);
    ctypedef int (*ControlledExit)(int, int, int, int, void*);
    ctypedef int (*SendData)(pvecvaluesall, int, int, void*);
    ctypedef int (*SendInitData)(pvecinfoall, int, void*);
    ctypedef int (*BGThreadRunning)(int, int, void*);
    ctypedef int (*GetVSRCData)(double*, double, char*, int, void*);
    ctypedef int (*GetISRCData)(double*, double, char*, int, void*);
    ctypedef int (*GetSyncData)(double, double*, double, int, int, int, void*);
    
    int  ngSpice_Init(SendChar* printfcn, SendStat* statfcn, ControlledExit* ngexit, 
                      SendData* sdata, SendInitData* sinitdata, BGThreadRunning* bgtrun, void* userData);
    int  ngSpice_Init_Sync(GetVSRCData* vsrcdat, GetISRCData* isrcdat, GetSyncData* syncdat, int* ident, void* userData);
    pvector_info ngGet_Vec_Info(char* vecname);
    int ngSpice_Circ(char** circarray);
    char* ngSpice_CurPlot();
    char** ngSpice_AllPlots();
    char** ngSpice_AllVecs(char* plotname);
    bint ngSpice_running();
    bint ngSpice_SetBkpt(double time);
