/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */
/* config.h
 *
 *  id3tool:  An editor for ID3 tags.
 *  Copyright (C) 1999-2000  Christopher Collins
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _CONFIG_H
#define _CONFIG_H

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Version Symbols */
#define PACKAGE "id3tool"
#define VERSION "1.1d"

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you have the getopt_long function.  */
#define HAVE_GETOPT_LONG 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the stricmp function.  */
/* #undef HAVE_STRICMP */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1
/*=======================================================================*/
/*                    DO NOT EDIT BELOW THIS POINT!!!!                   */
/*=======================================================================*/

#ifndef HAVE_STRDUP
extern char *	my_strdup (char *strin);
#define strdup(x) my_strdup(x)
#endif

#ifndef HAVE_STRCASECMP
#ifdef HAVE_STRICMP
#define strcasecmp(x,y) stricmp(x,y)
#else
#define strcasecmp(x,y) strcmp(x,y)
#endif /* HAVE_STRICMP */
#endif /* HAVE_STRCASECMP */

#ifndef HAVE_GETOPT_LONG
#include "getopt.h"
#endif

#endif
