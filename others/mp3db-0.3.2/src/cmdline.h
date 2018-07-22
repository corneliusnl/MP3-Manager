#ifndef _CMDLINE_H_
#define _CMDLINE_H_
// program notes 
// --------------
// the argtable library is replete with buffer overflows.
// The author states on his homepage that he plans on having this
// fixed in the next release of his library.  In the meantime,
// I have done my best to eliminate the conditions which cause 
// this library to overflow.  


#include <iostream>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include "../argtable/argtable.h"
#include <string.h>

using namespace std;

bool parsecmdline(int argc, char **argv);
char *getusername();
#endif
