/*
 * mp3html.h
 *
 * This holds all stuff that isn't actual code like structs, constants, and
 * globals.  A lot of this stuff is configurable, so it might behoove you to
 * check it out.
 */

#ifndef MP3HTML_H
#define MP3HTML_H

/* String Constants */
  const char NAME     [] = "MP3html";
  const char VERSION  [] = "1.3.6";
  const char DATE     [] = "Oct  5 1999";
  const char AUTHOR   [] = "Ben Canning";
  const char EMAIL    [] = "bdc@bdc.cx";
  const char HOMEPAGE [] = "http://www.bdc.cx/mp3html/";
  const char DESC     [] = "Make configurable lists of MP3s and does your dishes!";
  const char USAGE    [] = "-[DFfHhioprStu] template mp3_file(s)";


  /* I still don't have an actual use for this yet */
  const char LIST_TEMPLATE[] = "$ARTIST - $TITLE\n";


  /* Commandline options for use with getopt_long() */
  const struct option CMDLINE_OPTIONS[] =
  {
    {"help",        no_argument,       0, '1'},
    {"version",     no_argument,       0, '2'},
    {"trim-the",    no_argument,       0, '3'},
    {"stereo-text", required_argument, 0, '4'},
    {"mono-text",   required_argument, 0, '5'},
    {"header",      required_argument, 0, 'H'},
    {"footer",      required_argument, 0, 'F'},
    {"header-text", required_argument, 0, 'h'},
    {"footer-text", required_argument, 0, 'f'},
    {"template",    required_argument, 0, 't'},
    {"recursive",   required_argument, 0, 'r'},
    {"playlist",    required_argument, 0, 'p'},
    {"unknown",     required_argument, 0, 'u'},
    {"output",      required_argument, 0, 'o'},
    {"define",      required_argument, 0, 'D'},
    {"increment",   required_argument, 0, 'i'},
    {"space",       no_argument,       0, 'S'},
    {NULL,          0,                 0,   0}
  };



/* Define it as '\\' for MSDOS-based environments */
#ifdef MSDOS
  #define DIR_CHAR '\\'
#endif

#ifndef DIR_CHAR
  #define DIR_CHAR '/'
#endif


/* Use STREQ() instead of !strcmp */
#define STREQ !strcmp



/* Globals */
  int  increment_base = 1;
  long totalfilesize = 0;
  int  totalfiles = 0;
  long totalseconds = 0;

/* Commandline globals */
  char  underscores_for_spaces = 0;
  char  remove_leading_the = 0;
  char *unknown_text = NULL;
  char *stereo_text = "stereo";
  char *mono_text = "mono";





/*
 * mp3_node_struct, mp3_node, mp3_list
 *
 * These structs are used to build our linked list.  They hold information
 * like the filename, filesize, and contain structs which hold the header
 * data (MP3) and ID3 tags (ID3).  The routines to manipulate these structs
 * are in llist.c
 */
typedef struct mp3_node_struct mp3_node;
typedef mp3_node* mp3_list;

struct mp3_node_struct
{
  char* filename;
  long  file_length;
  char  has_id3;
  ID3   id3;
  MP3   mp3;

  mp3_node* next;
  mp3_node* prev;
};




/*
 * Function Prototypes
 *
 * These are (hopefully) all the functions we've got in all our source files.
 * The only functions which aren't declared here are the ones in libid3,
 * which are declared in 'libid3/libid3.h' and the ones from header.c which
 * are declared in 'header.h'
 */

  /* llist.c - Linked list routines */
  mp3_node* ll_first_node  (mp3_list);
  mp3_node* ll_last_node   (mp3_list);
  mp3_list  ll_add_node    (mp3_list, const char*);
  void      ll_delete_list (mp3_list);
  void      ll_delete_node (mp3_node*);

  /* misc.c - Miscellaneous functions*/
  char*    remove_leading_string (char *str, const char *lead);
  char*    load_template (const char*, char*);
  char*    int_to_char (const int);
  void     exit_message (const char*, const char*, const int);
  mp3_list load_playlist (mp3_list, const char*);
  mp3_list recurse_directory (mp3_list list, const char *dirname);
  void     show_commandline_help (const char* argv0);
  void     show_version (void);
  long     flen (FILE *work);
  char*    file_to_array (char* array, const char* filename);
  char*    replace_char (char *str, const char seek, const char repl);
  void     show_static_template (char* templt, FILE* output);

  /* subst.c - Functions to replace blocks of text */
  char* substitute_macro_tags( char* str );
  char* substitute_static_tags (char *str);
  char* substitute_tags (mp3_node* mp3, const char* mp3_template);
  char* substitute (char* str, const char* seek, const char* repl);

  /* macro.c - User-definable macros */
  void macro_add( char* );
  void macro_free( void );

  /* wrappers.c - Wrappers for functions */
  FILE* W_fopen (const char *filename, const char *mode);
  void* W_malloc (size_t size);
  void* W_realloc (void *ptr, size_t size);

#endif
/*EOF*/
