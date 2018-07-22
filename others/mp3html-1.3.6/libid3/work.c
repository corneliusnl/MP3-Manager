#include <stdio.h>
#include "libid3.c"

int
main (int argc, char **argv)
{
  int  loop1 = 1;
  FILE* f = NULL;
  char tmp;
  ID3 i;

  if (1 == argc)
      return 0;

  for (loop1 = 1; loop1 < argc; loop1++)
  {
     f = fopen (argv[loop1], "rw");
      if (0 == f)
          return 0;

      i = id3_read (f);
      fclose (f);
      id3_clean (&i);

      printf ("%s - %s\n", i.artist, i.title);
  }


  return 0;
}
