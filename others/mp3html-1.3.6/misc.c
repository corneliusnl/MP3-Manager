char *
remove_leading_string (char *str, const char *lead)
{
  char *new_start;

  if (!strncasecmp(str, lead, strlen(lead)))
  {
      new_start = &str[strlen(lead)];
      strcpy(str, new_start);
  }

  return &str[0];
}




/*
 * load_template()
 *
 * Loads a template passed by filename into an array.
 */
char*
load_template (const char* filename, char* tmplt)
{
  if (mp3_is_valid (fopen(filename,"rb")))
      exit_message ("This template looks a like a MP3", filename, 10);

  tmplt = file_to_array (tmplt, filename);

  if (NULL == tmplt)
      exit_message ("Can't load this template", filename, 10);

  return &tmplt[0];
}




/*
 * empty_string()
 *
 * Returns 0 if the string has stuff in it and a nonzero number if it's empty
 */
char
empty_string (const char* str)
{
  return !(str[0]);
}




/*
 * show_static_template()
 *
 * Writes a header or footer template to 'output' after substituting all the
 * static tags.
 */
void
show_static_template (char* tmplt, FILE* output)
{
  if (NULL != tmplt)
  {
      substitute_static_tags (tmplt);
      fputs (tmplt, output);
      fflush (output);

      if( '\n' != tmplt[strlen( tmplt ) - 1] )
          fputc( '\n', output );
  }

  return;
}




/*
 * replace_char()
 *
 * This goes through a char array (str) and replaces all occurences of seek
 * with repl.
 */
char*
replace_char (char *str, const char seek, const char repl)
{
  unsigned int loop1 = 0;

  for (loop1 = 0; str[loop1]; loop1++)
      if (str[loop1] == seek)
          str[loop1] = repl;

  return &str[0];
}


/*
 * filename_without_path()
 *
 * This little beauty goes back through a string and keeps the part from the
 * end until the first occurence of a '/', thus (hopefully) stripping off the
 * path part.
 */

char*
filename_without_path (char *path)
{
  static char* filename;
  long loop1;


  for (loop1 = strlen (path) - 1; DIR_CHAR != path[loop1] && loop1; loop1--);

  if (!loop1)
  {
      filename = path;
      return &filename[0];
  }

  filename = &path[loop1 + 1];
  return &filename[0];
}




/*
 * file_to_array()
 *
 * This reads a file specified by filename into a char array.  It handles all
 * memory allocation for you.
 */
char*
file_to_array (char* array, const char* filename)
{
  long  file_length;
  FILE* file;

  file = fopen (filename, "rb");
  if (0 == file)
      return NULL;
  
  file_length = flen( file );
  array = W_realloc (array, file_length);
  fread (array, 1, file_length, file);
  fclose (file);
  return &array[0];
}




/*
 * flen()
 *
 * This returns the length in bytes of an open file stream. It performs no
 * error checking itself, so don't trust it completely.
 */
long
flen (FILE *work)
{
  long tmp, length;

  tmp = ftell (work);
  fseek (work, 0L, SEEK_END);
  length = ftell (work);
  fseek (work, tmp, SEEK_SET);
  return length;
}




/*
 * int_to_char()
 *
 * This returns a pointer to a character array that contains the ascii
 * equivalent of 'num'.  Any subsequent calls to it will overwrite the old
 * array so be careful of that.
 */
char*
int_to_char (const int num)
{
  static char* str = NULL;
  int numtemp = num;
  int length = 0;


  for (length = 2; numtemp > 1; length++)
      numtemp /= 10;

  str = W_realloc (str, length);
  sprintf (str, "%d", num);

  return &str[0];
}




/*
 * exit_message
 *
 * Prints a supplied message, an optional filename, and exits with the
 * supplied exit_value.
 */
void
exit_message (const char* message, const char* file, const int exit_value)
{
  if (NULL == file)
      fprintf (stderr, "%s: %s (%d)\n", NAME, message, exit_value);
  else
      fprintf (stderr, "%s: (%s) %s (%d)\n", NAME, file, message, exit_value);

  exit (exit_value);
}




/*
 * show_commandline_help
 *
 * This function's name is pretty self explanatory.  It just shows the
 * commandline help screen and exits with errorlevel 0.
 */
void
show_commandline_help (const char* argv0)
{
  printf ("\n%s %s (%s)    by %s <%s>\n", NAME, VERSION, DATE, AUTHOR, EMAIL);
  printf ("  %s\n", DESC);
  printf ("  %s\n", HOMEPAGE);
  printf ("usage: %s %s\n\n", argv0, USAGE);
  printf (
      "  -D --define NAME=TEXT        Substitute all occurences of NAME with TEXT\n"
      "  -F --footer FILE             Read FILE into the footer template\n"
      "  -f --footer-text TEXT        Use TEXT as the footer template\n"
      "  -H --header FILE             Read FILE into the header template\n"
      "  -h --header-text TEXT        Use TEXT as the header template\n"
      "  -i --increment NUMBER        Set initial value of $INCREMENT to NUMBER\n"
      "     --mono-text TEXT          Text for $STEREO tag if MP3 is in mono\n"
      "  -o --output FILE             Write output to FILE (default is stdout)\n"
      "  -p --playlist FILE           Load files from playlist FILE (M3U)\n"
      "  -r --recursive DIR           Load directory tree starting in DIR\n"
      "  -S --space                   Replace underscores with spaces in filename\n"
      "     --stereo-text TEXT        Text for $STEREO tag if MP3 is in stereo\n"
      "  -t --template TEXT           Use TEXT as the middle template\n"
      "     --trim-the                Remove leading \"The \" from Artist name\n"
      "  -u --unknown TEXT            Replace empty ID3 fields with TEXT\n"
      "     --help                    Display this help and exit\n"
      "     --version                 Output version information and exit\n\n"
      "Try \"man mp3html\" for more information about these options.\n"
  );


  exit (0);
}




/*
 * show_version
 *
 * Shows the program name and version then exits with errorlevel 0.
 */
void
show_version (void)
{
  printf ("%s %s\n", NAME, VERSION);
  exit (0);
}




/*
 * recurse_directory
 *
 * Recursively scans a directory tree and adds any MP3s it finds to a list.
 */
mp3_list
recurse_directory (mp3_list list, const char *dirname)
{
  char fname[BUFSIZ];
  DIR *dir;
  struct dirent *d;


  if (NULL == dirname)
      exit_message ("No directory supplied for recursion.", NULL, 10);


  dir = opendir (dirname);
  if (!dir)
  {
      perror (dirname);
      exit (1);
  }

  while ( (d = readdir (dir)) )
  {
      if (strcmp (d->d_name, "..") && strcmp (d->d_name, "."))
      {
          if (dirname[strlen(dirname)-1] == DIR_CHAR)
              sprintf (fname, "%s%s", dirname, d->d_name);
          else
              sprintf (fname, "%s%c%s", dirname, DIR_CHAR, d->d_name);


          if (opendir (fname))
              list = recurse_directory (list, fname);

          else
              list = ll_add_node (list, fname);
      }
  }

  closedir (dir);
  return list;
}




/*
 * load_playlist
 *
 * As you probably guessed, this function loads a M3U playlist (which is just
 * a file with other filenames in it on seperate lines) into a list.
 */
mp3_list
load_playlist (mp3_list list, const char* playlist)
{
  char  fbuffer [BUFSIZ];
  FILE *f;


  f = W_fopen (playlist, "rt");

  if (mp3_is_valid( f ) )
      exit_message( "The playlist appears to be an MP3. Try again.",
                    playlist, 10);

  while ( fgets (fbuffer, sizeof fbuffer, f) )
  {
      fbuffer [strlen (fbuffer) - 1] = 0;    /* Trim the trailing '\n' */
      list = ll_add_node (list, fbuffer);
  }

  fclose (f);

  return list;
}

/*EOF*/
