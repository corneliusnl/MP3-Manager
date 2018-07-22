/*
 * libid3.h
 *
 * Header for libid3
 */

  #include "genre.h"

/*  #define STRING_COMPARE strcasecmp */ /* not ANSI, but it's nice */
  #define STRING_COMPARE strcmp


/*
 * typedef struct ID3
 */
typedef struct
{
  char title   [31];
  char artist  [31];
  char album   [31];
  char year    [5];
  char comment [31];
  unsigned char genre;
} ID3;


/* Function Prototypes */
  char id3_check (FILE*);
  char id3_check_by_filename (const char*);
  void id3_write (const ID3, FILE*);
  char id3_write_by_filename (const ID3, const char*);
  ID3  id3_read (FILE*);
  ID3  id3_read_by_filename (const char*);
  ID3  id3_clear (void);
  void id3_export (ID3, char*, char*, char*, char*, char*, unsigned char*);
  ID3  id3_create (const char*, const char*, const char*, const char*,
                   const char*, const char);
  char id3_string_to_genre (const char*);
  char id3_valid_genre (unsigned char);
  void id3_clean (ID3*);

/*EOF*/
