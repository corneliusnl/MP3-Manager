/*
 * Most of the code in this file was taken from MP3Info 0.2.15 by Thorvald
 * Natvig (slicer@bimbo.hive.no).  I brutally hacked it into plain C, added
 * the struct, and reformatted most of it.  But it's not really my work.
 */

/* #include "header.h"*/

const char*
mp3_mode (MP3 head)
{
  return (mode_names[head.mode]);
}




const char*
mp3_layer (MP3 head)
{
  return (layer_names[head.layer - 1]);
}




const char*
mp3_version (MP3 head)
{
  return (version_nums[head.version]);
}




MP3
mp3_read_header (FILE* file)
{
  unsigned char buff[1024];
  unsigned char *buffer;
  long offset;
  size_t temp;
  size_t readsize;
  MP3 head;

  /*
   * Theoretically reading 1024 instead of just 4 means a performance hit
   * if we transfer over net filesystems... However, no filesystem I know
   * of uses block sizes under 1024 bytes.
   */
  offset = mp3_find_header (file);
  fseek(file, offset, SEEK_SET);
  readsize = fread(&buff, 1, 1024, file);
  readsize -= 4;


  if (readsize <= 0)
      return (head);
 
  buffer = buff-1;

  /* Scan through the block looking for a header */
  do
  {
      buffer++;
      temp = ((buffer[0] << 4) & 0xFF0) | ((buffer[1] >> 4) & 0xE);
  } while ((temp != 0xFFE) && ((size_t)(buffer-buff)<readsize));

  if (temp != 0xFFE)
      return (head);

  switch ((buffer[1] >> 3 & 0x3))
  {
      case 3:
          head.version = 0;
          break;
      case 2:
          head.version = 1;
          break;
      case 0:
          head.version = 2;
          break;
      default:
          return (head);
  }

  head.layer            = 4 - ((buffer[1] >> 1) & 0x3);
  head.error_protection = !(buffer[1] & 0x1);
  head.bitrate          = (buffer[2] >> 4) & 0x0F;
  head.bitrate = bitrates[head.version][head.layer - 1][head.bitrate];
  head.frequency        = (buffer[2] >> 2) & 0x3;
  head.frequency = s_freq[head.version][head.frequency];
  head.padding          = (buffer[2] >> 1) & 0x01;
  head.extension        = buffer[2] & 0x01;
  head.mode             = (buffer[3] >> 6) & 0x3;
  head.mode_ext         = (buffer[3] >> 4) & 0x03;
  head.copyright        = (buffer[3] >> 3) & 0x01;
  head.original         = (buffer[3] >> 2) & 0x1;
  head.emphasis         = (buffer[3]) & 0x3;
  head.stereo           = (head.mode == 3) ? 1 : 2;

  if( 0 != head.bitrate )
      head.seconds      = (flen (file) / 128) / head.bitrate;
  else
      head.seconds      = 0; /* Necessary for some fscked-up MP3s */

  return (head);
}




char
mp3_is_valid (FILE *f)
{
  char buffer[3];
  long fmarker;   /* To preserve the file positioning */
  int  tmp;


  if (NULL == f)
      return 0;

  fmarker = ftell (f);
  fseek (f, 0, SEEK_SET);
  fread (buffer, 1, sizeof buffer, f);
  fseek (f, fmarker, SEEK_SET);

  tmp = ((buffer[0] << 4) & 0xFF0) | ((buffer[1] >> 4) & 0xE);

  if (0xFFE != tmp)
      return 0;

  else
      return 1;
}




long
mp3_find_header (FILE* f)
{
  char  buffer[3];
  long  offset;
  int   tmp;


  if (NULL == f)
      return -2;

  /* Should this be a cached read?  It wouldn't be very hard to do */
  for (offset = 0; offset < MAX_OFFSET; offset++)
  {
      fseek (f, offset, SEEK_SET);
      fread (buffer, 1, sizeof buffer, f);

      tmp = ((buffer[0] << 4) & 0xFF0) | ((buffer[1] >> 4) & 0xE);

      if (0xFFE == tmp)
          return( offset );
  }

  return -1;
}

/*EOF*/
