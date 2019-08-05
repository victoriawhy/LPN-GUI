/*
main.cc
-------
This code is based on the example code provided by Ngspice,
which can be dowloaded at this link: 

http://ngspice.sourceforge.net/ngspice-shared-lib/ngspice_cb.7z

TODO: error checking
*/

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <cmath>

#include <stdlib.h>
#include <stdio.h>
#define bool int
#define true 1
#define false 0
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>

#include "sharedspice.h"
#include "netlist.h"

using namespace std;

bool no_bg = true;
int numvecs = 0;
set<string> vecnames;
static bool errorflag = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
Netlist n;

int
ng_getchar(char* outputreturn, int ident, void* userdata);

int
ng_getstat(char* outputreturn, int ident, void* userdata);

int
ng_exit(int, bool, bool, int ident, void*);

int
ng_thread_runs(bool noruns, int ident, void* userdata);

int
ng_initdata(pvecinfoall intdata, int ident, void* userdata);

int
ng_data(pvecvaluesall vdata, int numvecs, int ident, void* userdata);

int
ng_getexternal(double* voltage, double t, char* node, int ident, void* userdata);

int
ciprefix(const char *p, const char *s);

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "Usage: ./main <file.cir>" << endl;
        return 0;
    }

    bool silent;
    if (argc > 2) {
        silent = (strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "--silent") == 0);
    }
    if (!silent) {
        cout << "Welcome to the command line LPN simulator" << endl;
        cout << "Print instructions? [y/n] ";
        string instructions;
        getline(cin, instructions);
        if (tolower(instructions[0]) == 'y') {
            cout << endl;
            cout << "This program loads the file provided into ngspice and runs the simulation specified in the file." << endl;
            cout << "If any of your elements require external input, you will be prompted to provide a file and a period." << endl;
            cout << "Your file must be formatted with each line: <time> <value>, where time and value are doubles. The period " << endl;
            cout << "should be a double that states the length of the file in time (commonly 1.0)." << endl;
            cout << endl;
            cout << "If you provide invalid files or period values, the behavior is undefined and the program may crash." << endl;
            cout << endl;
        }
    }

    // Create a Netlist object for this file.
    // This will prompt the user to enter files for any
    // external elements included in the file.
    n = Netlist();
    const string circuitfile = argv[1];
    n.load_from_file(circuitfile);

    int ret;
    
    // Initialize Ngspice
    ret = ngSpice_Init(ng_getchar, ng_getstat, ng_exit,  NULL, ng_initdata, ng_thread_runs, NULL);
    ret = ngSpice_Init_Sync(ng_getexternal, ng_getexternal, NULL, NULL, NULL);

    // Load netlist
    char **netlist_array = n.get_netlist();
    if (netlist_array == NULL) {
        cout << "Error loading netlist from file." << endl;
        cout << "Check your file and try again." << endl;
        cout << "Exiting..." << endl;
        return 0;
    }
    ret = ngSpice_Circ(netlist_array);
    
    // Run simulation
    pthread_mutex_lock(&mutex);
    ret = ngSpice_Command( (char*) "bg_run");

    // Wait for background thread to start
    while(no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }

    // Wait for background thread to exit
    while(!no_bg) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    /*
    * To customize this program, edit the code below. You likely want to use
    * the function ngSpice_Command((char *)"your ngspice commmand here") to send
    * regular ngspice commands. For a full description of how to use the shared
    * library API, check out chapter 19 of the Ngspice manual.
    */

    // Get vectors to save
    if (silent) {
        ret = ngSpice_Command((char*)"set filetype=ascii");
        ret = ngSpice_Command((char*)"write out.raw");
        cout << "All vectors saved to out.raw" << endl;
        cout << "Exiting..." << endl;
        return 0;
    }

    string save;
    cout << "Save output vectors? [y/n] ";
    getline(cin, save);
    while(save.length() != 1 || (tolower(save[0]) != 'y' && tolower(save[0]) != 'n')) {
        cout << "Please enter one of [y/n]." << endl;
        cout << "Save output vectors? [y/n] ";
        getline(cin, save);
    }
    if (tolower(save[0]) == 'n') {
        cout << "Exiting without saving..." << endl;
        return 0;
    }

    cout << "Available vectors are: " << endl;
    for (string name : vecnames) {
        cout << name << endl;
    }
    string format;
    cout << "Save format [a=ascii/b=binary]: ";
    getline(cin, format);
    while(format.length() != 1 || (tolower(format[0]) != 'a' && tolower(format[0]) != 'b')) {
        cout << "Please enter one of [a/b]." << endl;
        cout << "Save format [a/b]: ";
        getline(cin, format);
    }
    if (tolower(format[0]) == 'a') {
        ret = ngSpice_Command((char*)"set filetype=ascii");
        if (ret != 0) {
            cout << "Failed to set filetype to ASCII. Attempting to save in binary format.";
        }
    }
    string filename;
    cout << "Enter filename to save as: ";
    getline(cin, filename);
    cout << "Enter vectors to save, one per line, (or leave blank for all). Enter a blank line when you're done." << endl;
    string command = "write " + filename;
    string vec;
    while(true) {
        getline(cin, vec);
        if (vec.empty()) break;
        if (vecnames.find(vec) == vecnames.end()) {
            cout << "Vector not avaliable. Enter valid vectors to save and enter a blank line when you're done." << endl;
            continue;
        }
        command += vec;
    }

    cout << "Saving vectors. This may take a moment..." << endl;
    ret = ngSpice_Command(const_cast<char*>(command.c_str()));
    if (ret == 0) {
        cout << "Vectors saved." << endl;
    } else {
        cout << "There was a problem saving your vectors.";
    }

    cout << "Exiting..." << endl;

    return ret;
}

/********************************************************************************
NGSPICE CALLBACK FUNCTIONS

The following functions are used as callbacks for the Ngspice background thread
to communicate with the main thread of execution.

They are set using the calls to ngSpice_Init and ngSpice_InitSync above.

Chapter 19 of the manual contains more information about these callbacks.
*********************************************************************************/

/* Transfer any string created by printf or puts. 
Output to stdout in ngspice is preceded by token stdout, same with stderr. */
int
ng_getchar(char* outputreturn, int ident, void* userdata)
{
    printf("%s\n", outputreturn);
    /* setting a flag if an error message occurred */
    if (ciprefix("stderr Error:", outputreturn))
        errorflag = true;
    return 0;
}


/* Transfer status messages */
int
ng_getstat(char* outputreturn, int ident, void* userdata)
{
    printf("%s\n", outputreturn);
    return 0;
}

/* Called when the bg thread starts/stops running */
int
ng_thread_runs(bool noruns, int ident, void* userdata)
{   
    pthread_mutex_lock(&mutex);
    no_bg = noruns;
    if (noruns) {
        pthread_cond_signal(&cond);
        printf("bg not running\n");
    } else {
        pthread_cond_signal(&cond);
        printf("bg running\n");
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}


/* Called from bg thread in ngspice once upon intialization
   of the simulation vectors)*/
int
ng_initdata(pvecinfoall intdata, int ident, void* userdata)
{
    numvecs = intdata->veccount;
    for (int i = 0; i < numvecs; i++) {
        printf("Vector: %s\n", intdata->vecs[i]->vecname);
        vecnames.insert(intdata->vecs[i]->vecname);
    }
    return 0;
}


/* Callback function called from bg thread in ngspice if fcn controlled_exit()
   is hit. Do not exit, but unload ngspice. TODO: Is this right? */
int
ng_exit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{

    if(quitexit) {
        printf("DNote: Returned form quit with exit status %d\n", exitstatus);
        exit(exitstatus);
    }
    if(immediate) {
        printf("DNote: Unloading ngspice inmmediately is not possible\n");
        printf("DNote: Can we recover?\n");
    }

    else {
        printf("DNote: Unloading ngspice is not possible\n");
        printf("DNote: Can we recover? Send 'quit' command to ngspice.\n");
        errorflag = true;
        ngSpice_Command( (char*) "quit 5");
    }

    return exitstatus;
}

/* Called when the simulation needs a value from an external element */
int
ng_getexternal(double* voltage, double t, char* node, int ident, void* userdata)
{   
    *voltage = n.get_boundary_condition(node, t);
    return 0;
}

/* Case insensitive prefix. */
int
ciprefix(const char *p, const char *s)
{
    while (*p) {
        if ((isupper(*p) ? tolower(*p) : *p) !=
            (isupper(*s) ? tolower(*s) : *s))
            return(false);
        p++;
        s++;
    }
    return (true);
}

