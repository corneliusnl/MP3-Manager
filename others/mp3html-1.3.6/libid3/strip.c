#include <stdio.h>

int
strip_tag (long sizelesstag, char *fn)
{
  FILE *fd;

  fd = fopen(fn, "rw");

  if (fd == 0)
  {
    printf("strip_tag: Error opening %s", fn);
    return 0;
  }

#ifdef __WIN32__
  chsize(fd, sizelesstag);
#else
  ftruncate(fd, sizelesstag);
#endif

  close(fd);
  return 0;
}

