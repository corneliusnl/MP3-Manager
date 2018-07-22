/*********************************************************************
This file is part of the argtable library. It contains the declarations
of the argtable library functions.

Copyright (C) 1998,1999 Stewart Heitmann (Stewart.Heitmann@tip.csiro.au)

The argtable library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
USA.
*********************************************************************/
#ifndef ARGTABLE
#define ARGTABLE

#include <stdio.h>

#define ARGTABLE_VERSION 1.1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARGTABLE_COMPATIBILITY_10
#define arg_scanargv(a,b,c,d,e,f,g) arg_scanargv_10(a,b,c,d,e,f,g)
#endif

typedef enum { arg_int=0, arg_dbl, arg_str, arg_bool, arg_lit } arg_type;

extern const char* arg_typestr[]; 

typedef struct
  {
  const char *tagstr;     /* argument tag string */
  const char *argname;    /* argument name string */
  arg_type    argtype;    /* argument data type */
  void       *valueptr;   /* pointer to user-supplied storage location */
  const char *defaultstr; /* default argument value, as a string */
  const char *argdescrip; /* argument description string */
  } arg_rec;

extern void arg_catargs(int argc, char **argv, char *str);
extern void arg_dump(FILE* fp, const arg_rec* argtable, int n);
extern const char* arg_glossary(const arg_rec* argtable, int n, const char* prefix);
extern arg_rec arg_record(char *tagstr, char *argname, arg_type argtype, void *valueptr, char *defaultstr, char *argdescrip);
extern int arg_scanargv(int argc, char** argv, arg_rec *argtable, int n, char* CmdLine, char* ErrMsg, char* ErrMark);
extern int arg_scanstr(char* str, arg_rec *argtable, int n, char* ErrMsg, char* ErrMark);
extern const char* arg_syntax(const arg_rec* argtable, int n);

#ifdef __cplusplus
}
#endif
#endif









