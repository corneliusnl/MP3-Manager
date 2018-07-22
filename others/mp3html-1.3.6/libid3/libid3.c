#include "libid3.h"




/*
 * id3_read ()
 *
 * Reads a tag from mp3_file into an ID3 struct and returns it.
 */
ID3
id3_read (FILE* mp3_file)
{
  ID3 id3;


  id3 = id3_clear();

  if ( 0 == id3_check( mp3_file ) )
      return( id3 );
  else
  {
      fseek( mp3_file, -125, SEEK_END );

      fread( id3.title  , 1, 30, mp3_file );  id3.title[30]   = 0;
      fread( id3.artist , 1, 30, mp3_file );  id3.artist[30]  = 0;
      fread( id3.album  , 1, 30, mp3_file );  id3.album[30]   = 0;
      fread( id3.year   , 1,  4, mp3_file );  id3.year[4]     = 0;
      fread( id3.comment, 1, 30, mp3_file );  id3.comment[30] = 0;
      id3.genre = getc( mp3_file );


      id3_clean (&id3);
      return (id3);
  }
}




/*
 * id3_read_by_filename()
 *
 * Wrapper for id3_read that lets you lazy schmucks skip file handling.  It
 * returns a ID3 tag that will be blank if there was an error or none exists.
 */
ID3
id3_read_by_filename (const char* mp3_filename)
{
  FILE *mp3_file;
  ID3   id3;

  id3 = id3_clear();

  mp3_file = fopen (mp3_filename, "r+");
  if (0 == mp3_file)
      return id3;

  id3 = id3_read (mp3_file);
  fclose (mp3_file);

  return id3;
}




/*
 * id3_check()
 *
 * Checks to see if a file (MP3File) contains an ID3 tag.  Returns 0 if there
 * is no tag and a non-zero number if a tag exists.  Preserves file position.
 */
char
id3_check (FILE* mp3_file)
{
  long marker;
  char tmp[4];


  marker = ftell (mp3_file);
  fseek (mp3_file, -128, SEEK_END);
  fgets (tmp, 4, mp3_file);
  fseek (mp3_file, marker, SEEK_SET);

  return (!strcmp (tmp, "TAG") );
}




/*
 * id3_check_by_filename()
 *
 * Provides a wrapper for id3_check() so you lazy bastards don't have to
 * actually open the file.  Just pass in it's name.  Return values are the
 * same as id3_check() with the addition of '-1' to indicate failure when
 * opening the file.
 */
char
id3_check_by_filename (const char* mp3_filename)
{
  char  check_value;
  FILE* mp3_file;

  mp3_file = fopen (mp3_filename, "r+");
  if (0 == mp3_file)
      return -1;

  check_value = id3_check (mp3_file);
  fclose (mp3_file);

  return check_value;
}




/*
 * id3_write()
 *
 * Writes a tag (id3) to a file (mp3_file).  If there's already a tag, it's
 * overwritten.  If there isn't a tag in the file, it is appended.  As usual,
 * the current file position is preserved.
 */
void
id3_write (const ID3 id3, FILE* mp3_file)
{
  long marker;

  marker = ftell (mp3_file);

  /* If there's a tag already, go to the start of it for overwriting */
  if (0 != id3_check (mp3_file))
      fseek (mp3_file, -128, SEEK_END);
  /* If there isn't a tag, just go to the end of the file to append it */
  else
      fseek (mp3_file, 0L, SEEK_END);

  fwrite ("TAG",       1,  3, mp3_file);
  fwrite (id3.title,   1, 30, mp3_file);
  fwrite (id3.artist,  1, 30, mp3_file);
  fwrite (id3.album,   1, 30, mp3_file);
  fwrite (id3.year,    1,  4, mp3_file);
  fwrite (id3.comment, 1, 30, mp3_file);
  putc   (id3.genre,          mp3_file);

  fseek (mp3_file, marker, SEEK_SET);

  return;
}




/*
 * id3_write_by_filename()
 *
 * Provides a wrapper for id3_write() so you lazy bastards don't have to
 * actually open the file.  Just pass in it's name.  Only return value is -1
 * to signify an error opening the file.
 */
char
id3_write_by_filename (const ID3 id3, const char* mp3_filename)
{
  FILE *mp3_file;

  mp3_file = fopen (mp3_filename, "r+");
  if (0 == mp3_file)
      return -1;

  id3_write (id3, mp3_file);
  fclose (mp3_file);

  return 0;
}




/*
 * id3_create()
 *
 * Returns a new ID3 struct, filling the fields with the information passed
 * to it.  If you want a blank field, use NULL (except for genre).
 */
ID3
id3_create (const char *title, const char *artist, const char *album,
            const char *year, const char *comment, const char genre)
{
  ID3 id3;

  strcpy (id3.title,   title);
  strcpy (id3.artist,  artist);
  strcpy (id3.album,   album);
  strcpy (id3.year,    year);
  strcpy (id3.comment, comment);
  id3.genre = genre;

  return (id3);
}




/*
 * id3_clear()
 *
 * Returns a completely blank ID3 tag.
 */
ID3
id3_clear (void)
{
  ID3 id3;

  id3.title   [0] = 0;
  id3.artist  [0] = 0;
  id3.album   [0] = 0;
  id3.year    [0] = 0;
  id3.comment [0] = 0;
  id3.genre       = ID3_INVALID_GENRE;

  return (id3);
}




/*
 * id3_export()
 *
 * Exports a tag into external variables if you need it for some strange
 * reason.
 */
void
id3_export (ID3 id3, char* title, char* artist, char* album, char* year,
            char* comment, unsigned char* genre)
{
  strcpy (title,   id3.title);
  strcpy (artist,  id3.artist);
  strcpy (album,   id3.album);
  strcpy (year,    id3.year);
  strcpy (comment, id3.comment);
  *genre = id3.genre;

  return;
}




/*
 * id3_string_to_genre()
 *
 * Takes a string (genre) and returns the cooresponding genre value.  If none
 * is found, it returns -1.
 */
char
id3_string_to_genre (const char *genre)
{
  int genre_loop;

  for (genre_loop = 0; genre_loop < ID3_NUMBER_OF_GENRES; genre_loop++)
      if (0 == STRING_COMPARE (genre, id3_genre[genre_loop]))
          return genre_loop;

  return -1;
}




/*
 * id3_valid_genre()
 *
 * Returns true if the parameter could be used to specify a valid genre.
 */
char
id3_valid_genre (unsigned char genre)
{
  return (genre < ID3_NUMBER_OF_GENRES);
}




/*
 * id3_clean()
 *
 * Removes the extra spaces that programs like WinAMP like to use to pad the
 * tags.  Scans and cleans 'artist', 'title', 'album', and 'comment'.
 */
void
id3_clean (ID3 *id3)
{
  int last;

  for (last = strlen (id3->artist) - 1; id3->artist[last] == ' '; last--)
      id3->artist[last] = 0;

  for (last = strlen (id3->title) - 1; id3->title[last] == ' '; last--)
      id3->title[last] = 0;

  for (last = strlen (id3->album) - 1; id3->album[last] == ' '; last--)
      id3->album[last] = 0;

  for (last = strlen (id3->comment) - 1; id3->comment[last] == ' '; last--)
      id3->comment[last] = 0;

  for (last = strlen (id3->year) - 1; id3->year[last] == ' '; last--)
      id3->year[last] = 0;


  return;
}

/*EOF*/
