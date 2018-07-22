#include <string.h>
#include <stdio.h>
#include "libid3.c"

/*
Petr Strnad saw the problems in this so he decided to make a lyrics tag,
enabling the text to reside inside the audio file. This is done by creating
a chunk of data which begins with "LYRICSBEGIN", ends with "LYRICSEND" and
has the lyrics between these keywords. This data block is then saved in the
audio file between the audio and the ID3 tag.

  The keywords "LYRICSBEGIN" and "LYRICSEND" must not be present in the lyrics.
  The text is encoded with ISO-8859-1 character set
  A byte in the text must not have the binary value 255.
  The maximum length of the lyrics is 5100 bytes.
*/

/*
  lyrics3_read
  lyrics3_check
  lyrics3_write
*/

char
lyrics3_check( FILE* mp3_file )
{
  char buffer[10];


  if( 0 == id3_check( mp3_file ) )
      return 0;

  fseek (mp3_file, -137, SEEK_END);
  fread (buffer, 1, sizeof buffer, mp3_file);
  buffer[9] = 0;

  return( ! strcmp( buffer, "LYRICSEND" ) );
}



void
lyrics3_read( FILE* mp3_file )
{
  char buffer[5101];
  char* start;
  char* end;

  if( 0 == mp3_file || 0 == lyrics3_check( mp3_file ) )
      return;

/*  fseek( mp3_file, (-128) - (-5100) - (-9) - (-11) , SEEK_END );*/
  fseek( mp3_file, 0L, SEEK_SET);
  fread( buffer, 1, 5100, mp3_file );

  start = strstr( buffer, "LYRICSBEGIN" );
  start += 11;
  end = strstr( start, "LYRICSEND" );
  end[0] = 0;

  puts( start );
  return;
}




int
main (int argc, char** argv)
{
  int argloop;

  for (argloop = 1; argloop < argc; argloop++)
      lyrics3_read( fopen( argv[argloop], "rb" ) );

  return 0;
}
