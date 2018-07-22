#ifndef HEADER_H
#define HEADER_H

#define MAX_OFFSET 1024*128  /* Check the first 128kb for MP3 header */


/*
 * MP3
 *
 * This struct holds the information gotten from the MP3 header.  It has
 * technical stuff like bitrate, layer, version, etc.  The routines to read
 * this struct are in 'header.c'.
 */
typedef struct
{
  int version;
  int layer;
  int error_protection;
  int bitrate;
  int frequency;
  int padding;
  int extension;
  int mode;
  int mode_ext;
  int copyright;
  int original;
  int emphasis;
  int stereo;
  int seconds;
} MP3;



/* Function prototypes from header.c */
  const char*  mp3_mode        (MP3);
  const char*  mp3_layer       (MP3);
  const char*  mp3_version     (MP3);
  MP3          mp3_read_header (FILE*);
  char         mp3_is_valid    (FILE*);
  long         mp3_find_header (FILE*);



const char*
mode_names[5] =
{
    "stereo", "j-stereo", "dual-ch", "single-ch", "multi-ch"
};


const char*
layer_names[3] =
{
  "I", "II", "III"
};



const char*
version_nums[3] =
{
  "1", "2", "2.5"
};


const unsigned int
bitrates[3][3][15] =
{
  {
      {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448},
      {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384},
      {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320}
  },
  {
      {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160}
  },
  {
      {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160},
      {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160}
  }
};


const unsigned int
s_freq[3][4] =
{
  {44100, 48000, 32000, 0},
  {22050, 24000, 16000, 0},
  {11025, 8000,   8000, 0}
};

#endif
/*EOF*/
