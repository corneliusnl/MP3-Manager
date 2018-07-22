/*
 * include.c
 *
 * This file tells MP3html which source files it needs to compile.
 */

/* System includes */
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <getopt.h>

#ifdef __FreeBSD__
  #include <sys/types.h>
#endif  /* Thanks to Sebestyen Zoltan for this patch */


/* Our own includes */
#include "libid3/libid3.c"
#include "header.h"
#include "mp3html.h"
#include "header.c"
#include "misc.c"
#include "wrappers.c"
#include "llist.c"
#include "macro.c"
#include "subst.c"

/*EOF*/
