/*********************************************************************
This file is part of the argtable library. It contains the definitions
of the argtable library functions as well as specially formatted
comments that constitute the source text for the manual pages
pertaining to those functions.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "argtable.h"

#define ARGSTRLEN 200
#define NAMESTRING(A) ((A.argname)?(A.argname):(arg_typestr[A.argtype]))
#define NULLSAFE(S) ((S)?(S):(""))


static
const char whitespace[] = " \f\n\r\v\t";

static
int arg_convertbool(const char *str)
  {
  /************************************************************
  Returns 0 if *str satisfies a case-insensitive match on a 
  leading substring of "FALSE", "NO", or "OFF". Leading whitespace
  is ignored.
  Returns 1 if *str matches "TRUE", "YES", or "ON".
  Returns -1 otherwise.
  Handles str==NULL safely.
  **************************************************************/
  char buff[ARGSTRLEN];
  int i,n;

  /*-- handle NULL str */
  if (str==NULL)
     return -1;

  /*-- skip leading whitespace --*/
  while (isspace((int)*str))
     str++;

  /*-- handle empty string --*/
  if (strcmp(str,"")==0)
     return -1;

  /*-- put an upper case copy of *str in buff[] --*/
  n = strlen(str);
  strncpy(buff,str,ARGSTRLEN);
  for (i=0; i<n; i++)
     buff[i]=toupper(str[i]);

  /*-- match on "FALSE", "NO", or "OFF" --*/
  if (strncmp(buff,"FALSE",n)==0)
     return 0;
  if (strncmp(buff,"NO",n)==0)
     return 0;
  if (strncmp(buff,"OFF",n)==0 && n>1)
     return 0;

  /*-- match on "TRUE", "YES", or "ON" --*/
  if (strncmp(buff,"TRUE",n)==0)
     return 1;
  if (strncmp(buff,"YES",n)==0)
     return 1;
  if (strncmp(buff,"ON",n)==0 && n>1)
     return 1;

  /*-- no match --*/
  return -1;
  }


static
int arg_set_defaults(arg_rec *argtable, int n, char* ErrMsg)
  {
  /***************************************************************************
  Converts the default argument values from strings into the appropriate data
  type for that argument. 
  Argument table entries with NULL default strings are ignored. 
  Default values  of arguments with NULL valueptrs are converted but then
  discarded. This is for consistent behaviour, rather than absolute necessity.

  Should a default value be invalid, then an error message is written into
  *ErrMsg and the function returns 0. Should ErrMsg==NULL then the error
  message is omitted.
  The funtion returns 1 upon success.
  ***************************************************************************/
  int i;

  /*-- clear *ErrMsg --*/
  if (ErrMsg)
     ErrMsg[0]='\0';

  for (i=0; i<n; i++)
     {
     /*-- skip over NULL defaults --*/
     if (argtable[i].defaultstr==NULL)
        continue;

     /*-- convert default value to appropriate data type --*/
     switch (argtable[i].argtype)
       {
       case arg_bool:
	  {
          int x = arg_convertbool(argtable[i].defaultstr);
          if (!(x==0 || x==1))
	     {
             if (ErrMsg)
                 sprintf(ErrMsg,"arg[%d].defaultstr == \"%s\", invalid bool",
                         i,argtable[i].defaultstr);
             return 0;
	     }
          if (argtable[i].valueptr!=NULL)
             *((int*)argtable[i].valueptr) = x;
          break;
          }

       case arg_int:
       case arg_lit:
	  {
          char *p=NULL;
          int x;          
          x = (int)strtol(argtable[i].defaultstr,&p,0);
          /* printf("errno %d, p \"%s\"\n",errno,p); */
          if (errno!=0 || strlen(p)>0 || p==argtable[i].defaultstr)
	     {
             if (ErrMsg)
                 sprintf(ErrMsg,"arg[%d].defaultstr == \"%s\", invalid int",
                         i,argtable[i].defaultstr);
             return 0;
	     }
          if (argtable[i].valueptr!=NULL)
             *((int*)argtable[i].valueptr) = x;
	  break;
	  }

       case arg_dbl:
	  {
          char *p;
          double x;          
          x = strtod(argtable[i].defaultstr,&p);
          if (errno!=0 || strlen(p)>0 || p==argtable[i].defaultstr)
	     {
             if (ErrMsg)
                 sprintf(ErrMsg,"arg[%d].defaultstr == \"%s\", invalid double",
                         i,argtable[i].defaultstr);
             return 0;
	     }
          if (argtable[i].valueptr!=NULL)
             *((double*)argtable[i].valueptr) = x;
	  break;
	  }

       case arg_str:
          if (argtable[i].valueptr!=NULL)
             strcpy((char*)(argtable[i].valueptr), argtable[i].defaultstr);
          break;

       };

     }

  return 1;  
  }


static
int arg_scanf(const char* str, arg_rec* argrec)
  {
  /*************************************************************
  Scans *src for the argument specified in *argrec and writes 
  the result into *(argrec->valueptr). The function returns the
  number of characters that were read. 
  If (argrec->valueptr)==NULL then the scanned value is discarded.
  Should an error occur, the function returns -1 and leaves
  *(argrec->valueptr) unaltered.
  Note: Strings which are enclosed by single quotes are automatically
  stripped of the enclosing quotes.
  **************************************************************/
  int length = 0;

  switch (argrec->argtype)
    {
    case arg_bool:
      {
      char boolstr[ARGSTRLEN];
      int  boolval;

      /*-- scan a boolean string value --*/
      if ( sscanf(str,"%s%n",boolstr,&length) != 1 )
         return -1;

      /*-- convert string to boolean int value --*/
      boolval = arg_convertbool(boolstr);
      if (boolval==-1)
         return -1;

      /*-- write scanned value into *argrec --*/
      if (argrec->valueptr)
         *((int*)argrec->valueptr) = boolval;
      break;
      }

    case arg_lit:
      {
      /*-- if arg has a tag then presume it was located just prior to   --*/
      /*-- this function. Thus we have found a literal tag. Set the arg --*/
      /*-- value (presuming it also is non-NULL) to 1.                  --*/
      if (argrec->tagstr && argrec->valueptr)
         *((int*)argrec->valueptr) = 1;   

      /*-- if argname!=NULL then scan a literal from the command line.   --*/
      if (argrec->argname)
	 {
	 /*char fmtstr[ARGSTRLEN];*/
         char litstr[ARGSTRLEN];
 
         /*-- scan a literal string value --*/
         if ( sscanf(str,"%s%n",litstr,&length) != 1 )
            return -1;

         /*-- compare literal value to expected value --*/
         /*-- (ignores possibility of leading whitespace in argname) --*/
         if (strcmp(argrec->argname,litstr)!=0)
            return -1;

         /*-- scan succeeded --*/
         if (argrec->valueptr)
            *((int*)argrec->valueptr) = 1;
         /*printf("...ok (%d)\n",length);*/
	 }
      break;
      }

    case arg_int:
      {
      int intval;

      /*-- scan an int value --*/
      if (sscanf(str,"%i %n",&intval,&length) !=1)
         return -1;

      /*-- write scanned value into *argrec --*/
      if (argrec->valueptr)
         *((int*)argrec->valueptr)  = intval;      
      break;
      }

    case arg_dbl:
      {
      double dblval;

      /*-- scan for double value --*/
      if ( sscanf(str,"%lf%n",&dblval,&length) != 1)
         return -1;

      /*-- write scanned value into *argrec --*/
      if (argrec->valueptr)
         *((double*)argrec->valueptr)  = dblval;      
      break;
      }

    case arg_str:
      {
      char strval[ARGSTRLEN];

      /*-- scan for quoted string fist, if no good then try unquoted --*/
      if (sscanf(str," '%[^']'%n",strval,&length) != 1)
         if (sscanf(str,"%s%n",strval,&length) != 1)
            return -1;

      /*-- write the string into *argrec --*/
      if (argrec->valueptr)
         strcpy((char*)argrec->valueptr,strval);
      break;
      }
    }

  return length; 
  }


static
char* arg_extract_tag(char* str, const char* tag)
  {
  /**********************************************************************
  Searches 'str' for the first occurrence of 'tag'.
  If found, the tag is erased from str by overwriting it with
  spaces. The function then returns a pointer to the next char in 'str'
  immediately after the tag.
  If the tag cannot be found in cmdline then NULL is returned.
  **********************************************************************/
  char* p = strstr(str,tag);
  if (p)
    {
    int n = strlen(tag);
    memset(p,' ',n);
    p+=n;
    }
  return p;
  }



static
char* arg_extract_value(char* str, arg_rec* argrec)
  {
  /************************************************************
  Scans the leading characters of str for an argument value as
  specified by 'argrec'. If successful, the scanned value is
  written into *(argrec->valueptr). The scanned characters are then
  erased from str by overwriting them with spaces. The function
  then returns a pointer to the next char in 'str' following the
  scanned chars.
  If argrec->valueptr==NULL then the scanned value is discarded.
  If the scan was unsuccessful, both 'str' and *(argrec-valueptr)
  remain unaltered and the function returns NULL.
  **************************************************************/
  int nscan = arg_scanf(str, argrec);
  if (nscan==-1)
     return NULL;
  memset(str,' ',nscan);
  return (str+nscan);
  }


static
void arg_sprint_marker(int i, int n, char *ErrMark)
  {
  /************************************************************
  Writes i leading spaces, followed by n carets '^' into *ErrMark.
  **************************************************************/
  while (i-->0)
    *ErrMark++ = ' ';
  while (n-->0)
    *ErrMark++ = '^';
  *ErrMark='\0';
  }


static
int arg_extract_tagged_args(char* cmdline, arg_rec* argtable, int n, 
                            char* ErrMsg, char* ErrMark)
  {
  /****************************************************************
  Steps thru each entry in the argument table. For each entry that
  has a non-NULL tag string, the string in *cmdline is scanned for
  a sequence of chars that match the tag. If found, the argument
  value that is associated with the tag is scanned from *cmdline
  and written into the memory location pointed to by the argument
  table entry. 
  Having done so, the tag + value is then erased from *cmdline by
  overwriting them with spaces.
  If the valueptr field of the argument table entry is NULL then
  the scanned value is discarded.
  The function returns 1 upon success, 0 upon failure.
  If the tag is not found in *cmdline and there is no default
  argument value for that argument table entry then an error is
  returned.
  If the tag is found, but the argument value could not be scanned
  then an error is returned.
  If multiple occurrences of the tag are found then an error is
  returned.
  Error messages are written into a user supplied string buffer at
  *ErrMsg.
  A marker string that indicates the position of the error in *cmdline
  is written into a user supplied string buffer at *ErrMark.
  Both ErrMsg and ErrMark are optional. If they are given as NULL
  they are ignored.
  ****************************************************************/
  int i;

  /*-- init ErrMsg and ErrMark if given non-NULL ptrs --*/
  if (ErrMsg)
     *ErrMsg = '\0';
  if (ErrMark)
     *ErrMark = '\0';

  /*-- iterate thru the tagged entries in argtable[]--*/
  for (i=0; i<n; i++) 
     {
     const char* argtag  = NULLSAFE(argtable[i].tagstr);
     const char* argname = NAMESTRING(argtable[i]);
     char *p;
     /*printf("scanning arg %d in \"%s\"\n",i,cmdline);*/

     /*-- skip entries with NULL tags --*/
     if (argtable[i].tagstr==NULL)
        continue;        

     /*-- extract 1st occurrence of argument tag from cmdline string.    --*/
     /*-- if successful, the return char* points to the char immediately --*/
     /*-- following the tag that just extracted from *cmdline.           --*/
     p = arg_extract_tag(cmdline,argtable[i].tagstr);

     /*-- if tag was not found then... --*/
     if (!p)
        {
        /*-- if a default argument value is supplied then skip, else error --*/
        if (argtable[i].defaultstr!=NULL)
           continue; 
        else
           {
           if (ErrMsg) 
              sprintf(ErrMsg,"missing %s%s argument", argtag,argname);
           if (ErrMark)
              arg_sprint_marker(strlen(cmdline),1,ErrMark);
           return 0;
           }
	}

     /*-- tag was found, now scan the arg value --*/
     if (!arg_extract_value(p,&argtable[i]))
        {
        if (ErrMsg) 
           sprintf(ErrMsg,"invalid %s%s argument", argtag,argname);
        if (ErrMark)
           arg_sprint_marker((int)(p-cmdline),1,ErrMark);
        return 0;
        }
 
     /*-- check for other occurrences of same tag --*/
     p = arg_extract_tag(cmdline,argtable[i].tagstr);
     if (p)
        {
        if (ErrMsg) 
           sprintf(ErrMsg,"repeat tag");
        if (ErrMark)
	   {
           int taglen = strlen(argtable[i].tagstr);
           arg_sprint_marker((int)(p-cmdline-taglen),taglen,ErrMark);
	   }
        return 0;
        }
     }

  return 1;
  }
  

static
int arg_extract_untagged_args(char* cmdline, arg_rec* argtable, int n, char* ErrMsg, char * ErrMark)
  {
  /****************************************************************
  Steps thru each entry in the argument table. For each entry that
  has a NULL tag string, cmdline[] is scanned from left to right for
  the first occuring argument. The value of that argument is written
  into the location specified by the valueptr field of the appropriate
  argtable entry. The argument is then erased from cmdline[] by
  overwriting it with spaces.
  If the valueptr field is NULL then the scanned value is discarded.
  Upon successful completion of the function, all scanned arguments
  will have been erased from cmdline[]. Any extraneous arguments
  remain in cmdline[] intact.
  If an argument value does not match the expected type and that 
  argument is optional (ie: it has a non-NULL default value) then
  we give the user the benefit of the doubt and assume that the
  value given was not intended for this argument. On the other
  hand, if the argument is not optional, then we know immediately
  that the value is invalid and an error should be generated.
  Error messages are written into a user supplied string buffer at
  *ErrMsg.
  A marker string that indicates the position of the error in *cmdline
  is written into a user supplied string buffer at *ErrMark.
  Both ErrMsg and ErrMark are optional. If they are given as NULL
  they are ignored.
  The function returns 1 upon success, 0 upon failure.
  ****************************************************************/
  char* p = cmdline;
  int i;

  /*-- init ErrMsg and ErrMark if given non-NULL ptrs --*/
  if (ErrMsg)
     *ErrMsg = '\0';
  if (ErrMark)
     *ErrMark = '\0';

  /*-- iterate thru each untagged entry in argtable[] --*/
  for (i=0; i<n; i++) 
     {
     const char* argname = NAMESTRING(argtable[i]);

     /*printf("scanning arg %d in \"%s\"\n",i,cmdline);*/

     /*-- skip entries with non-NULL tags --*/
     if (argtable[i].tagstr!=NULL)
        continue;        

     /*-- skip leading whitespace --*/
     while (isspace((int)*p))
        p++;

     /*-- if there are no more arguments on the command line then... --*/
     if (*p=='\0')
        {
        /*-- if a default argument is given then skip arg, else error --*/
        if (argtable[i].defaultstr!=NULL)
           continue;
	else
	   {
           if (ErrMsg) 
              sprintf(ErrMsg,"%s argument expected", argname);
           if (ErrMark)
	      {
              int m = strspn(cmdline,whitespace);
              arg_sprint_marker(m,1,ErrMark);
	      }
           return 0;
           }
	}

     /*-- extract leading argument value from cmdline string --*/
     /*-- if successfull then skip to next arg --*/
     if (arg_extract_value(cmdline,&argtable[i]))
        continue;

     /*-- we get to here only if argument value extraction failed. --*/        
     /*-- if the arg is optional (ie: has a default) then we presume that --*/
     /*-- the value was not intended for this arg, so skip to next arg.   --*/
     if (argtable[i].defaultstr!=NULL)
         continue;

     /*-- we get here only if the command line value is invalid and   --*/
     /*-- the expected argument is not optional. the value given must --*/
     /*-- therefore be erroneous so generate an error and return zero --*/
     if (ErrMsg) 
        sprintf(ErrMsg,"invalid %s argument", argname);
     if (ErrMark)
        {
        int m = strspn(cmdline,whitespace);
        int n = strcspn(cmdline+m,whitespace);
        arg_sprint_marker(m,n,ErrMark);
	}
     return 0;
     }

  return 1;
  }
 


static
int arg_checktable(arg_rec *argtable, /* ptr to argument table */
                   int n,             /* number of entries in argtable[] */
                   char *ErrMsg)      /* ptr to target error string (may be NULL)*/
  {
  /************************************************************
  Checks the validity of an argument table.

  DESCRIPTION:
  Each of the 'n' entries in 'argtable[]' are tested for validity;
  any errors will cause a one-line description of the failure to be
  written into *ErrMsg, after which the function will return zero.
  *ErrMsg is assumed to be big enough to hold the result, unless it
  is given as NULL in which case the error messages are suppressed.

  There is little, or no, reason to call this function directly as
  both arg_scanargv() and arg_scanstr() call it automatically.

  The function also converts the default value of each argument from
  a string into its appropriate data type and stores the result in
  the location designated  for that argument. Any invalid default
  values are reported in the same manner as invalid arguments.

  RETURN VALUE:
  Returns 1 if the argument table is valid, otherwise returns 0.

  VALIDITY CONSTRAINTS:
  An argument tag can be NULL, but it cannot be an empty string or 
  comprise entirely of whitespace. Whitespace is allowed provided
  there is at least one non-whitespace character in the tag.

  **************************************************************/
  int i;

  for (i=0; i<n; i++)
     {
     /*-- check validity of non-NULL tag strings --*/
     if (argtable[i].tagstr!=NULL)
        {
        if (strcmp(argtable[i].tagstr,"")==0 ||
            strcspn(argtable[i].tagstr,whitespace)==0)
           {
           if (ErrMsg)
              sprintf(ErrMsg,"arg[%d], invalid tag \"%s\"",i,argtable[i].tagstr);
           return 0;
           }
	}
     }

  /*-- check and set defaults --*/
  if (!arg_set_defaults(argtable,n,ErrMsg))
     return 0;

  return 1;
  }






/*=============== publicly accessable functions follow ===================*/



void arg_catargs(int argc,        /* number of arguments in argv[] */
                 char **argv,     /* ptr to array of argument strings */
                 char *str)       /* ptr to target string */
  {
  /************************************************************
  Concatenate all argv[] arguments.

  DESCRIPTION:
  Concatenates all of the arguments in the argv[] array and writes the
  result into *str as a single line, space separated string.
  Any argv[] entries that contain whitespace are automatically
  encapsulated by single quotes prior to the concatenation to preserve
  their word grouping. 
  A trailing space is always appended to the resulting string as a
  safety precaution in lieu of scanning for string literals that expect
  trailing space.
  It is assumed that *str is big enough to store the result.
  
  Ordinarily you would not call this function yourself. It is supplied 
  in case you wish to process some string other than argv[], or perhaps
  you may wish to bypass arg_scanargv() altogether and use arg_scanstr()
  to process the command line manually.
  **************************************************************/
  int i;
  str[0]='\0';
  for (i=0; i<argc; i++)
     {
     /*-- if argv[i] has any whitespace then... --*/
     if (strpbrk(argv[i],whitespace))
        {
        strcat(str,"'");
        strcat(str,argv[i]);
        strcat(str,"'");
        }
     else
        strcat(str,argv[i]);       

     strcat(str," ");
     }
  }



void arg_dump(FILE* fp,	               /* output stream  */
	      const arg_rec* argtable, /* ptr to argument table */
	      int   n)	               /* number of entries in argtable[] */
  {
  /************************************************************
  Displays the contents of an argument table.
  
  DESCRIPTION:
  The contents of the argument table, and the user-supplied
  variables it references,  are printed to the stream 'fp'. 
  This can be useful for debugging argument tables.
  **************************************************************/
  int i;

  for (i=0; i<n; i++)
    {
    char tag[ARGSTRLEN], name[ARGSTRLEN], type[ARGSTRLEN], value[ARGSTRLEN],
         descr[ARGSTRLEN];

    /*-- tag --*/
    if (argtable[i].tagstr)
       sprintf(tag, "\"%s\"", argtable[i].tagstr);
    else
       sprintf(tag,"NULL");

    /*-- name --*/
    if (argtable[i].argname)
       sprintf(name, "\"%s\"", argtable[i].argname);
    else
       sprintf(name,"NULL");

    /*-- type and value--*/
    sprintf(value, "NULL");
    switch (argtable[i].argtype)
      {
      case arg_int:
        sprintf(type,"arg_int");
        if (argtable[i].valueptr)
           sprintf(value, "%d", *((int*)argtable[i].valueptr));
        break;
      case arg_dbl:
        sprintf(type,"arg_dbl");
        if (argtable[i].valueptr)
           sprintf(value, "%f", *((double*)argtable[i].valueptr));
        break;
      case arg_str:
        sprintf(type,"arg_str");
        if (argtable[i].valueptr)
           sprintf(value, "\"%s\"", (char*)argtable[i].valueptr);
        break;
      case arg_bool:
        sprintf(type,"arg_bool");
        if (argtable[i].valueptr)
           sprintf(value, "%d", *((int*)argtable[i].valueptr));
        break;
      case arg_lit:
        sprintf(type,"arg_lit");
        if (argtable[i].valueptr)
           sprintf(value, "%d", *((int*)argtable[i].valueptr));
        break;
      };

    /*-- description --*/
    if (argtable[i].argdescrip)
       sprintf(descr,"\"%s\"",argtable[i].argdescrip);
    else
       sprintf(descr,"NULL");

    fprintf(fp,"%3d: %-15s %-15s %10s = %-10s %s\n", i, tag, name, type, value, descr);
    }
  }



const char* arg_glossary(const arg_rec* argtable,  /* ptr to argument table */
                         int n,          /* number of entries in argtable[] */ 
                         const char* prefix)   /* a string to be prefixed to each line of the output */
  {
  /************************************************************
  Generate an argument table glossary
  
  DESCRIPTION:
  Returns a pointer to an internal 'glossary' string which
  contains a multi-line description of each of the argument table
  entres that have a non-NULL <description> field. 
  The contents of the glossary string remain unaltered up until the
  next invocation of this function.
  Each line of the glossary string is formatted as

  "<prefix><tag><name><description>"

  The 'prefix' string is useful for adding indenting spaces
  before each line in the description to improve the look of
  the glossary string, or it can be given as NULL in which
  case it is ignored.

  Any NULL <tag> fields in the argument table will appear in the
  glosssary as empty strings.

  Any NULL <name> fields will be substituted by a description of
  that argument's data type, enclosed in angled brackets, as in
  <int> and <double>. 
  A name can effectively be suppressed from the glossary by defining
  it as an empty string in the argument table.
  **************************************************************/
  static char str[3000];
  static char NULLprefix[]="";
  int i;

  /*-- handle case of NULL prefix string --*/
  if (prefix==NULL)
     prefix=NULLprefix;

  /*-- initialise str to "" --*/
  str[0]='\0';

  for (i=0; i<n; i++)
    if (argtable[i].argdescrip!=NULL) 
      {
      char tempstr[ARGSTRLEN]="";

      sprintf(tempstr, "%s%s%s%s\n", prefix, NULLSAFE(argtable[i].tagstr),
              NAMESTRING(argtable[i]), argtable[i].argdescrip);
      strcat(str,tempstr);
      }
  return (const char*)str;
  }



arg_rec arg_record(char *tagstr,      /* argument tag string */
                   char *argname,     /* argument name string */
                   arg_type argtype,  /* argument data type */
                   void *valueptr,    /* pointer to user-supplied storage location */
                   char *defaultstr,  /* default argument value, as a string */
                   char *argdescrip)  /* argument description string */
  {
  /************************************************************
  Builds and returns an argument table record.
  
  DESCRIPTION:
  Returns an arg_rec structure containing the values passed to
  the function. It is useful for building argument tables dynamically.
  **************************************************************/
  arg_rec result;
  result.tagstr     = tagstr;
  result.argname    = argname;
  result.argtype    = argtype;
  result.valueptr   = valueptr;
  result.defaultstr = defaultstr;
  result.argdescrip = argdescrip;
  return result;
  }



int (arg_scanargv)(int argc,	    /* number of entries in 'argv'. */
                 char **argv,	    /* ptr to command line arguments. */
                 arg_rec *argtable, /* ptr to argument table. */
                 int n,	            /* number of entries in argtable[]. */
                 char* CmdLine,     /* ptr to storage for command line (may be NULL). */
                 char* ErrMsg,      /* ptr to storage for error message (may be NULL). */
                 char* ErrMark)     /* ptr to storage for error marker (may be NULL). */
  {
  /************************************************************
  Scan the command line as per the given argument table.
  
  DESCRIPTION:
  Attempts to resolve the command line arguments given in argv[]
  with the specifications given in the argument table. 
  The values scanned from the command line are written directly into
  the chosen memory locations as specified by each argument table
  entry. Notice that the program name, in argv[0], is not treated as
  an argument and does not require an entry in the argument table.
  
  During the process, a copy of the command line is written
  (as a single line of space separated arguments) into the 
  user-supplied string at *CmdLine in case it is needed in future
  for error reporting.

  Should there be any conflict between the command line arguments
  and the argument table specifications, an error message and
  corresponding error marker are written into the user-supplied
  strings at *ErrMsg and *ErrMark respectively, after which the
  function returns zero. The error marker string is used to store
  a string of tilde characters formated in such a way that the tildes
  underscore the exact location of the error in *CmdLine when the
  strings are aligned one above the other. This can be useful for
  including in on-line error messages to help the user quickly
  pinpoint the cause of the error.
  
  If, on the other hand, all arguments were resolved successfully
  then *ErrMsg and *ErrMark are set to empty strings and the 
  function returns 1.
  Either way, CmdLine, ErrMsg, or ErrMark can be safely ignored
  by passing them as NULL.


  RETURN VALUE:	
  Returns 1 upon success, 0 upon failure.


  THE ARGUMENT TABLE:
  The argument table is an array of arg_rec structs, and has one entry
  for each command line argument that is expected. 
  It is most conveniently defined and initialised as a
  static array within the main block, but need not be so.

  We know that the first argument in argv[0] is always going to be the
  name of the executable so arg_scanargv() skips it during the scanning
  phase, starting instead with argv[1]. Thus the first entry of an
  argument table refers to argv[1] and not argv[0] (This was not the
  case in argtable version 1.0).


  ANATOMY OF AN ARGUMENT TABLE ENTRY:
  The arg_rec structure has six members; a tag string, an argument name, 
  an argument type, a pointer to a user-supplied data location, a default
  value (as a string), and a description string.
  A typical table entry might look like,

  { "--bs", "<size>", arg_int, &blcksize, "1024", "\\ttape block size" }
  
  Here, we have a tagged integer argument whose value is to be stored
  in the program variable called "blcksize". It would be used on the
  command line as --bs 2048 for example.
  In our case, the argument has a default value of 1024 that is 
  converted from a string to an integer and written into blcksize
  whenever a corresponding argument cannot be found on the command line.
  If the default value was instead given as NULL then the argument would
  be regarded as mandatory.

  Because our argument has a tag it may, unlike untagged arguments, be used
  anywhere on the command line. 
  Furthermore, there need not be any space between the tag and the
  subsequent argument value, so --bs 2048 and --bs2048 would both be
  accepted. You can force the presence of whitespace between the
  tag and the value by putting a trailing space in the tag string, as in
  "--bs ". The choice is yours.

  The argument name "<size>" has no effect on the argument processing.
  It is just a descriptive name used to represent the argument in the
  output of the arg_syntax() and arg_glossary() functions, which in our
  case would appear as "--bs<size>". 
  The argument name can be whatever you want, and is a convenient place
  to communicate the default value to the user if you so wish,
  for example "<size>=1024". If instead you specify a NULL name string,
  it is automatically replaced by the arguments data type enclosed in 
  angled brackets, as in "<integer>". If you dislike such behaviour, 
  you can effectively suppress the name by defining it as an empty string.

  Like the name string, the description string "\\ttape block size" does not
  affect the processing. It is only used by the arg_glossary function, which
  in our case would produce a glossary entry of 
  "--bs<size>      tape block size".
  Arguments with NULL description strings are omitted from the glossary
  altogether.
  See arg_syntax() and arg_glossary() for more details on the name and
  description fields.

  An argument may also be untagged, as in the following string
  argument,

  { NULL, "<infile>", arg_str, infilename, NULL, " input file" }

  This argument has no tag and no default value and stores its result in the
  user-defined string called "infilename".
  Untagged arguments are expected to appear on the command line in the
  same order as they are defined in the argument table.
  When tagged and untagged arguments are mixed, the argument table is
  processed in two passes. The first pass extracts all the tagged 
  arguments from the command line, the second pass extracts the 
  remaining untagged arguments from left to right.
 
  As a final note, argtable entries may be given a NULL in place of the
  pointer-to-void entry that specifies the location of the user-defined
  program variable. Such arguments are still scanned from the command line in
  the normal way, but the resulting value is discarded. 


  ARGUMENT DATA TYPES:
  It is imperative that the data type of program variable used to store
  the argument's value matches the enum arg_type given in the third
  column of the argument table. If you get this wrong you can you can
  expect very spurious behaviour from the argtable functions.

  The enum arg_type is defined as

  typedef enum {arg_int, arg_dbl, arg_str, arg_bool, arg_lit} arg_type;

  Its values correspond to integer, double, string and boolean data types
  followed by a special string literal type. The boolean type stores its
  value as an integer that is either 0 or 1. The arg_lit type is a special
  case which is discussed in detail later, for now it is suffice to say that
  it too stores an integer value.

  The arg_bool type does not read an integer value from the command line,
  rather it expects one of the keywords "true", "false", "yes", "no", "on",
  or "off" which it then converts to 0 (false,no,off) or 1 (true,yes,on).
  For example, the argument 

  { "debug:", NULL, arg_bool, &dbg, "false", "\\tdebugging option" }

  defines an optional tagged boolean argument whose default value is 0 (false).
  It may appear on the command line as any of debug:true, debug:yes,
  debug:on, debug:false, debug:off, or debug:no. 
  The value 0 or 1 is written into the integer program variable "dbg". 
  
  The arg_lit type represents a special argument type. Unlike the other
  arguments, it does not scan a parameterised value from the command line.
  Instead, it just expects a given string literal to be 
  present on the command line, as in --verbose or --help.
  The string literal can be defined in either the tag or the name string
  fields. If you use the tag then the string literal, like other tagged
  arguments, may appear anywhere on the command line. On the other hand,
  if you use the name string, then the string literal must appear in that
  argument's position just as for normal untagged arguments.
  When a string literal is succesfully scanned, an integer value of 1 is
  written into its user-suplied program variable, otherwise it is assigned
  the default value (if it has one). If there is no default value, then the
  literal is regarded as a mandatory argument the same as for any other
  argument.

  The following arg_lit argument shows how to implement a --verbose command
  line switch,

  { "--verbose ", NULL, arg_lit, &verbosity, "0", "\\tverbosity switch" }

  Should --verbose be given on the command line, the value 1 is written
  into the integer program variable named "verbosity", otherwise the default
  value of 0 is used instead.

  Another use of the arg_lit type is to enforce the presence of a given
  keyword on the command line. This is useful to help distinguish individual
  command line usages when multiple candidate argument tables can be
  be applied.  For example, the argtable entry

  { NULL, "geometric", arg_lit, NULL, NULL, NULL }

  has no tag, no default, no storage location, and no description. It
  simply defines the string literal "geometric" which must be present at
  a specific location on the command line for the argument table to be
  succesfully resolved.  A sister argument table might have the entry

  { NULL, "linear", arg_lit, NULL, NULL, NULL }

  in the same argument position. Thus, the keywords "geometric"
  and "linear" are used to distinguish the two possible usages.
  **************************************************************/
  char cmdline[10000], *p;

  /*-- init CmdLine, ErrMsg and ErrMark if given --*/
  if (CmdLine) *CmdLine='\0';
  if (ErrMsg)  *ErrMsg='\0';
  if (ErrMark) *ErrMark='\0';

  /*-- concat arguments in argv[] into one line in cmdline[] --*/
  arg_catargs(argc,argv,cmdline);

  /*-- give user a copy of cmdline if requested --*/
  if (CmdLine)
      strcpy(CmdLine,cmdline);

  /*-- check argtable validity and convert default values --*/
  if (!arg_checktable(argtable,n,ErrMsg))
     return 0;
  
  /*-- erase the first word, it is the program name and we dont want it. --*/
  /*-- dont forget enclosing quotes on prog names containing spaces!     --*/
  if (strpbrk(argv[0],whitespace))
     memset(cmdline,' ',strlen(argv[0])+2);
  else
     memset(cmdline,' ',strlen(argv[0]));
   
  /*-- extract arguments from cmdline[] --*/
  if (!arg_scanstr(cmdline,argtable,n,ErrMsg,ErrMark))
     return 0;

  /*-- check cmdline[] for extraneous arguments --*/
  p = cmdline;
  while (isspace((int)*p))
     p++;
  if (*p!='\0')
     {
     if (ErrMsg) 
        sprintf(ErrMsg,"unexpected argument");
     if (ErrMark)
	{
        int n = strcspn(p,whitespace);
        arg_sprint_marker((int)(p-cmdline),n,ErrMark);
	}
     return 0;
     }

  return 1;
  }



int (arg_scanargv_10)(int argc, char **argv, arg_rec *argtable, int n,
                     char* CmdLine, char* ErrMsg, char* ErrMark)
  {
  char cmdline[10000], *p;

  /*-- init CmdLine, ErrMsg and ErrMark if given --*/
  if (CmdLine) *CmdLine='\0';
  if (ErrMsg)  *ErrMsg='\0';
  if (ErrMark) *ErrMark='\0';

  /*-- concat arguments in argv[] into one line in cmdline[] --*/
  arg_catargs(argc,argv,cmdline);

  /*-- give user a copy of cmdline if requested --*/
  if (CmdLine)
      strcpy(CmdLine,cmdline);

  /*-- check argtable validity and convert default values --*/
  if (!arg_checktable(argtable,n,ErrMsg))
     return 0;

  /*-- extract arguments from cmdline[] --*/
  if (!arg_scanstr(cmdline,argtable,n,ErrMsg,ErrMark))
     return 0;

  /*-- check cmdline[] for extraneous arguments --*/
  p = cmdline;
  while (isspace((int)*p))
     p++;
  if (*p!='\0')
     {
     if (ErrMsg) 
        sprintf(ErrMsg,"unexpected argument");
     if (ErrMark)
	{
        int n = strcspn(p,whitespace);
        arg_sprint_marker((int)(p-cmdline),n,ErrMark);
	}
     return 0;
     }

  return 1;
  }



int arg_scanstr(char* str,         /* ptr to command line string. */
                arg_rec *argtable, /* ptr to argument table. */
                int n,             /* number of entries in argtable[]. */
                char* ErrMsg,      /* ptr to storage for error message (may be NULL). */
                char* ErrMark)     /* ptr to storage for error marker (may be NULL). */
  {
  /************************************************************
  Scan a command line string as per the given argument table.
  
  DESCRIPTION:
  This function is much like arg_scanargv() except that is scans
  the arguments from the string at *str rather than from argv[]. 
  The string is expected to contain a single line, space separated
  list of arguments, like that generated by arg_catargs().

  In a departure from arg_scanargs(), this function erases the scanned
  arguments from *str by overwriting them with spaces once they have been
  successfully scanned.
  Furthermore, this function does not throw an error if there are still
  arguments remaining in *str after the argtable has been fully processed.
  Thus, complicated argument usages can be achieved by invoking this
  function multiple times on the same command line string,
  each time applying a different argument table until the arguments
  have been exhausted, or an error has been detected.

  RETURN VALUE:	
  Returns 1 upon success, 0 upon failure.
  **************************************************************/

  /*-- init ErrMsg and ErrMark if given --*/
  if (ErrMsg)  *ErrMsg='\0';
  if (ErrMark) *ErrMark='\0';

  /*-- check argtable validity and convert default values --*/
  if (!arg_checktable(argtable,n,ErrMsg))
     return 0;
  
  /*-- extract tagged arguments from str --*/
  if (!arg_extract_tagged_args(str,argtable,n,ErrMsg,ErrMark))
     return 0;

  /*-- extract untagged arguments from str --*/
  if (!arg_extract_untagged_args(str,argtable,n,ErrMsg,ErrMark))
     return 0;

  return 1;
  }



const char* arg_syntax(const arg_rec* argtable,  /* ptr to argument table */
                       int n)          /* number of entries in argtable[] */ 
  {
  /************************************************************
  Generates a 'usage' syntax string from an argument table
  
  DESCRIPTION:
  Builds a syntactical description of the allowable command line arguments
  specified by the 'argtable' array.
  The resulting string is stored in static data within the local scope of
  this function. Its contents are overwritten by subsequent calls.

  The syntactical description is generated as a single line of space
  separated argument descriptors, each comprising of the argument's tag
  string and name string concatenated together. For example,

  "myprog x y [z] [-r <double>] [-o <outfile>] [-verbose] <infile> [debug=<on/off>]"

  If an argument is optional (has a non-NULL default value) then its
  descriptor is enclosed in square brackets. 
  NULL name strings are substituted with the argument's data type enclosed
  in angled brackets, as in <int>, <double>, or <string>. 
  If both the tag and the name are empty strings ("") then the argument is
  omitted from the description altogether. This allows the suppression of
  individual arguments that you do not want to appear.

  RETURN VALUE:	
  Returns a pointer to the syntax string.
  **************************************************************/
  static char str[ARGSTRLEN];
  int addspace=0;
  int i;

  /*-- initialise str to "" --*/
  str[0]='\0';

  /*-- concatenate consecutive argument descriptions to str.          --*/
  /*-- ignore those arguments whose tag and name strings are both "". --*/
  for (i=0; i<n; i++)
    if (strcmp(NULLSAFE(argtable[i].tagstr),"")!=0 ||
        strcmp(NAMESTRING(argtable[i]),"")!=0)
      {
      const int has_default = (argtable[i].defaultstr==NULL) ? 0 : 1;
    
      /*-- prefix all but first argument with space separator --*/
      if (addspace)
        strcat(str," ");
      else
        addspace=1;

      /*-- write opening [ for optional arguments--*/
      if (has_default)
        strcat(str,"[");

      /*-- write tag string --*/
      strcat(str,NULLSAFE(argtable[i].tagstr));

      /*-- write arg name --*/
      strcat(str,NAMESTRING(argtable[i]));

      /*-- write closing ] for optional args --*/
      if (has_default)
        strcat(str,"]");
      }

  return (const char*)str;
  }


/************************************************************
Fixed array of data type names.

DESCRIPTION:
Defines a fixed array of strings that can be used as substitute names
for arguments with NULL names. The array is indexed by arg_type; each
name describing the corresponding data type

arg_str[arg_int]  = "<int>"

arg_str[arg_dbl]  = "<double>"

arg_str[arg_str]  = "<string>"

arg_str[arg_bool] = "<bool>"

arg_str[arg_lit]  = ""

The name for arg_lit has been deliberately left empty because literals
do not have stored data value as such.
**************************************************************/
const char* arg_typestr[] = { "<int>", "<double>", "<string>", "<bool>", "" };


