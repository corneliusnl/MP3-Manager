char*
substitute (char* str, const char* seek, const char* repl)
{
  char*  temp_string;
  int    start;
  int    str_len;
  int    seek_len;


  if (!str || !seek || !repl || STREQ(seek, repl))
      return str;

  while (strstr (str, seek))
  {
      str_len = strlen (str);
      seek_len = strlen (seek);
/*      str = W_realloc (str, str_len + strlen(repl) - seek_len + 1);*/

      start = str_len - strlen((char*)strstr(str,seek));

      temp_string = malloc (str_len - start - seek_len + 1);
      strcpy (temp_string, &str[start + seek_len]);

      str[start] = 0;

      strcat (str, repl);
      strcat (str, temp_string);

      free (temp_string);
  }

  return &str[0];
}




char*
substitute_macro_tags( char* str )
{
  int macro_loop;

  for (macro_loop = 0; macro_loop < macro_num; macro_loop++)
      substitute( str, macro_name[macro_loop], macro_data[macro_loop]);

  return &str[0];
}




char*
substitute_static_tags (char *str)
{
  char time_seconds[3];
  char avgtime_seconds[3];
/*
  time_t current_time;

  time (&current_time);
  substitute( str, "$TIME", ctime(&current_time));
*/

  substitute_macro_tags( str );

  sprintf(time_seconds, "%02ld", totalseconds % 60);
  sprintf(avgtime_seconds, "%02ld", (totalseconds/totalfiles) % 60);

  substitute(str, "$MP3HTMLVERSION" , VERSION);
  substitute(str, "$MP3HTMLDATE"    , DATE);
  substitute(str, "$MP3HTMLAUTHOR"  , AUTHOR);
  substitute(str, "$MP3HTMLEMAIL"   , EMAIL);
  substitute(str, "$MP3HTMLHOMEPAGE", HOMEPAGE);
  substitute(str, "$TOTALFILESIZEKB", int_to_char(totalfilesize / 1024) );
  substitute(str, "$TOTALFILESIZEMB", int_to_char(totalfilesize / (1024 * 1024) ) );
  substitute(str, "$TOTALFILESIZE"  , int_to_char(totalfilesize) );
  substitute(str, "$AVGFILESIZEKB"  , int_to_char(totalfilesize / 1024 / totalfiles) );
  substitute(str, "$AVGFILESIZEMB"  , int_to_char(totalfilesize / (1024 * 1024) / totalfiles) );
  substitute(str, "$AVGFILESIZE"    , int_to_char(totalfilesize / totalfiles) );
  substitute(str, "$TOTALLENGTHSECONDS", time_seconds );
  substitute(str, "$TOTALLENGTHMINUTES", int_to_char(totalseconds / 60) );
  substitute(str, "$TOTALLENGTH"       , int_to_char(totalseconds) );
  substitute(str, "$AVGLENGTHMINUTES",   int_to_char(totalseconds / 60 / totalfiles));
  substitute(str, "$AVGLENGTHSECONDS",   avgtime_seconds);
  substitute(str, "$AVGLENGTH",          int_to_char(totalseconds));
  substitute(str, "$TOTALFILES"        , int_to_char(totalfiles) );

  return &str[0];
}




char*
substitute_tags (mp3_node* mp3, const char* mp3_template)
{
  static char* tmp = NULL;
  static int increment = -32767;
  char time_seconds[3];


  /* -32767 is used as an invalid number (although it isn't really) */
  if( increment == -32767 )
      increment = increment_base;

  /* SLOPPY! Shouldn't be static, dammit. */
  tmp = W_realloc( tmp, strlen( mp3_template ) + 4000);
  strcpy (tmp, mp3_template);

  substitute_static_tags (tmp);

  /* Add a trailing '\n' if needed */
  if (tmp[strlen(tmp)-1] != '\n')
      strcat (tmp, "\n");

  /* Make sure $LENGTHSECONDS will always be 2 characters long */
  sprintf (time_seconds, "%02d", mp3->mp3.seconds % 60);

  /* Take care of "-S --space" */
  if (underscores_for_spaces)
      replace_char (mp3->filename, '_', ' ');


  /* Put unknown_text into ID3 tag where needed */
  if (0 == mp3->id3.artist[0])
      strcpy (mp3->id3.artist, unknown_text);
  if (0 == mp3->id3.album[0])
      strcpy (mp3->id3.album, unknown_text);
  if (0 == mp3->id3.comment[0])
      strcpy (mp3->id3.comment, unknown_text);


  /* Substitute the tags */
  substitute( tmp, "$FILENAME"   , filename_without_path(mp3->filename) );
  substitute( tmp, "$PATHFILENAME", mp3->filename );
  substitute( tmp, "$VERSION"    , mp3_version(mp3->mp3) );
  substitute( tmp, "$LAYERROMAN" , mp3_layer(mp3->mp3) );
  substitute( tmp, "$MODE"       , mp3_mode(mp3->mp3) );
  substitute( tmp, "$LAYER"      , int_to_char(mp3->mp3.layer) );
  substitute( tmp, "$BITRATE"    , int_to_char(mp3->mp3.bitrate) );
  substitute( tmp, "$FREQUENCY"  , int_to_char(mp3->mp3.frequency) );
  substitute( tmp, "$FILESIZEMB" , int_to_char(mp3->file_length / (1024000)));
  substitute( tmp, "$FILESIZEKB" , int_to_char(mp3->file_length / 1024) );
  substitute( tmp, "$FILESIZE"   , int_to_char(mp3->file_length) );
  substitute( tmp, "$LENGTHMINUTES", int_to_char(mp3->mp3.seconds / 60) );
  substitute( tmp, "$LENGTHSECONDS", time_seconds);
  substitute( tmp, "$LENGTH"       , int_to_char(mp3->mp3.seconds) );
  substitute( tmp, "$INCREMENT"    , int_to_char(increment++) );

  if (mp3->mp3.stereo == 2)
      substitute(tmp, "$STEREO", stereo_text);
  else
      substitute(tmp, "$STEREO", mono_text);

  /* ID3 Tags */
  substitute( tmp, "$ARTIST",  mp3->id3.artist);
  substitute( tmp, "$ALBUM",   mp3->id3.album);
  substitute( tmp, "$COMMENT", mp3->id3.comment);

  if (mp3->has_id3)
      substitute( tmp, "$TITLE", mp3->id3.title);
  else
      substitute( tmp, "$TITLE", filename_without_path(mp3->filename) );

  if (mp3->id3.genre > ID3_NUMBER_OF_GENRES)
  {
      substitute( tmp, "$GENRENUM", "");
      substitute( tmp, "$GENRE"   , unknown_text);
  }
  else
  {
      substitute( tmp, "$GENRENUM", int_to_char(mp3->id3.genre) );
      substitute( tmp, "$GENRE"   , id3_genre[mp3->id3.genre]);
  }

  if (!mp3->id3.year[0])
      substitute( tmp, "$YEAR", unknown_text);
  else
      substitute( tmp, "$YEAR", mp3->id3.year);


  return &tmp[0];
}
/*EOF*/
