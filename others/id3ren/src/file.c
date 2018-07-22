/* id3 Renamer
 * file.c - File i/o functions
 * Copyright (C) 1998  Robert Alto (badcrc@tscnet.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */


#include <stdio.h>
/* open */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "file.h"
#include "id3ren.h"
#include "misc.h"


/* todo: add option to change log filename */
const char logfile[] = "id3ren.log";


int
add_to_log (char *data)
{
  FILE *fp;

  if (id3_open_file(&fp, (char*)logfile, "at") == FALSE)
    return FALSE;

  fprintf(fp, "%s", data);
  fclose(fp);

  return TRUE;
}


int
id3_read_file (char *dest, unsigned long size, FILE *fp, char *fn)
{
  if (fread(dest, size, 1, fp) != 1)
  {
    print_error("id3_read_file: Error reading %s", fn);
    fclose(fp);
    return FALSE;
  }

  if (ferror(fp) != 0)
  {
    print_error("id3_read_file: Error reading %s", fn);
    clearerr(fp);
    fclose(fp);
    return FALSE;
  }

  if (feof(fp))
  {
    print_error("id3_read_file: Premature end of file in %s", fn);
    fclose(fp);
    return FALSE;
  }

  return TRUE;
}


int
id3_write_file(char *src, unsigned long size, FILE *fp, char *fn)
{
  if (fwrite(src, size, 1, fp) != 1)
  {
    print_error("id3_write_file: Error writing to %s", fn);
    fclose(fp);
    return FALSE;
  }

  if (ferror(fp) != 0)
  {
    print_error("id3_write_file: Error writing to %s", fn);
    clearerr(fp);
    fclose(fp);
    return FALSE;
  }

  return TRUE;
}


int
id3_open_file (FILE **fp, char *fn, char *mode)
{
  *fp = fopen(fn, mode);

  if (*fp == NULL)
  {
    print_error("id3_open_file: Error opening file %s", fn);
    return FALSE;
  }

  return TRUE;
}


int
id3_seek_header (FILE *fp, char *fn)
{

  if (fseek(fp, -128, SEEK_END) < 0)
  {
    fclose(fp);
    print_error("id3_seek_header: Error reading file %s", fn);
    return FALSE;
  }

  return TRUE;
}


int
strip_tag (long sizelesstag, char *fn)
{
  int fd;

  fd = open(fn, O_RDWR);

  if (fd == -1)
  {
    print_error("strip_tag: Error opening %s", fn);
    return FALSE;
  }

#ifdef __WIN32__
  chsize(fd, sizelesstag);
#else
  ftruncate(fd, sizelesstag);
#endif

  close(fd);
  return TRUE;
}

