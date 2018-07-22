/* id3 Renamer
 * id3tag.c - id3 tag manipulation functions
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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "genre.h"
#include "misc.h"
#include "id3ren.h"
#include "id3tag.h"
#include "file.h"


extern FLAGS_struct flags;
extern char *program_name;

extern char *def_artist;
extern char *def_song;
extern char *def_album;
extern char *def_year;
extern char *def_comment;
extern int def_genre;

ID3_tag *ptrtag = NULL;

void
show_genres (int pause_flag)
{
  int i, count, lines;

  for (i=0, count=1, lines=1; i<genre_count; i++, count++)
  {
    printf("%3d:%-15s", (i+1), genre_table[i]);

    if (count >= 4)
    {
      printf("\n");
      count = 0;
      lines++;
      if (lines >= (get_term_lines()-1))
      {
        if (pause_flag)
        {
          printf("-MORE- Press ENTER");

          if (getc(stdin) != '\n')
            while (getc(stdin) != '\n');

          lines = 0;
        }
      }
    }
  }

  if (count > 1)
    printf("\n");

}


/* flag_search_only:
 * 0 - Display & prompt for genres, exact match returns TRUE
 * 1 - Prompt, exact match returns TRUE
 * 2 - No prompt, show only sub string matches
 */
int
search_genre (int flag_search_only, int *dest, char *search_gen)
{
  int i;
  int digit;
  int substring_matches;
  char c = 'z';

  i = 0;

  while (i < strlen(search_gen) && isdigit(search_gen[i]))
    i++;

  if (i >= strlen(search_gen))
  {
    digit = (atoi(search_gen) - 1);

    if (digit >= 0 && digit < genre_count)
    {
      if (flag_search_only == 2)
        user_message(FALSE, "%s\n", genre_table[digit]);

      *dest = digit;
      return TRUE;
    }

    user_message(TRUE, "%s: search_genre: Invalid genre type: %d\n",
      program_name, digit);
    *dest = -1;
    return FALSE;
  }

  substring_matches = 0;

  for (i = 0; i < genre_count; i++)
  {
    if (flag_search_only != 2 &&
      strcasecmp(search_gen, genre_table[i]) == 0)
    {
      *dest = i;

      if (flag_search_only == 0 && flags.verbose)
        printf("Found exact genre match [%s]\n", genre_table[i]);

      return TRUE;
    }

    /* keep track of substring matches for later */

    if (strcase_search(genre_table[i], search_gen))
    {
      substring_matches++;

      if (flag_search_only == 0 && flags.verbose)
        printf("Found matching genre [%s]\n", genre_table[i]);

      if (flag_search_only == 2)
        printf("%3d: %s\n", substring_matches, genre_table[i]);
    }
  }

  if (substring_matches == 0)
  {
    user_message(FALSE, "Genre not found: %s\n", search_gen);
    *dest = -1;
    return FALSE;
  }

  if (flag_search_only == 2)
    return TRUE;

  /* HMMMM....didn't find an exact match, now search for sub strings */

  for (i = 0; i < genre_count && c != 'q'; i++)
  {
    if (strcase_search(genre_table[i], search_gen))
    {
      if (substring_matches == 1)
      {
        *dest = i;
        return TRUE;
      }

      do
      {
        printf("[%s] use this genre? (Y/n/q) ", genre_table[i]);
        c = tolower(getchar());

        if (c == '\n')
        {
          c = 'y';
        }
        else
        {
          while (getchar() != '\n') ;
        }
      } while (c != 'n' && c != 'y' && c != 'q');

      if (c == 'y')
      {
        *dest = i;
        return TRUE;
      }
    }
  }

  if (flag_search_only == 0)
    user_message(FALSE, "No genre selected...tough choice isn't it?\n");

  *dest = -1;
  return FALSE;
}

void
resize_tag_field (char *field)
{
  int i;

  i = strlen(field);
  while (field[i-1] == ' ')
    i--;
  field[i] = '\0';

}

int
get_tag_genre (int *genre, int def_genre)
{
  char buffer[64];
  char c;
  int flag_got_genre = FALSE;

  if (def_genre >= 0 && def_genre < genre_count)
  {
    *genre = def_genre;
    printf("Genre of music: %s\n", genre_table[*genre]);
    return TRUE;
  }

  if (flags.edit_tag && *genre >= 0 && *genre < genre_count)
  {
    printf("Genre of music: %s\n", genre_table[*genre]);
    return TRUE;
  }

  do
  {
    printf("Genre of music (blank to cancel, 'l' to list): ");
    fgets(buffer, sizeof(buffer), stdin);

    if (buffer[strlen(buffer)-1] == '\n')
    {
      buffer[strlen(buffer)-1] = '\0';
    }
    else
    {
      while (getc(stdin) != '\n');
    }

    if (tolower(buffer[0]) == 'l')
      show_genres(TRUE);
    else if (strlen(buffer) < 1)
      flag_got_genre = TRUE;
    else if (search_genre(FALSE, &(*genre), buffer) == TRUE)
      flag_got_genre = TRUE;
    else
    {
      do
      {
        printf("Would you like to see a list? (Y/n) ");
        c = tolower(getchar());
      } while (c != 'n' && c != 'y' && c != '\n');

      if (c == '\n')
        c = 'y';
      else
        while (getchar() != '\n') ;

      if (c == 'y')
        show_genres(TRUE);
    }
  } while (flag_got_genre == FALSE);

  if (*genre != -1)
    printf("\"%s\" selected.\n", genre_table[*genre]);

  return TRUE;
}


int
write_tag (int append_flag, char *fn)
{
  FILE *fp;

  if (append_flag == TRUE)
  {
    if (id3_open_file(&fp, fn, "ab") == FALSE)
      return FALSE;
  }
  else
  {
    if (id3_open_file(&fp, fn, "r+b") == FALSE)
      return FALSE;

    if (id3_seek_header(fp, fn) == FALSE)
      return FALSE;
  }

  strcpy(ptrtag->tag, "TAG");
  if (!id3_write_file(ptrtag->tag, (sizeof(ptrtag->tag)-1), fp, fn))
    return FALSE;
  if (!id3_write_file(ptrtag->songname, (sizeof(ptrtag->songname)-1), fp, fn))
    return FALSE;
  if (!id3_write_file(ptrtag->artist, (sizeof(ptrtag->artist)-1), fp, fn))
    return FALSE;
  if (!id3_write_file(ptrtag->album, (sizeof(ptrtag->album)-1), fp, fn))
    return FALSE;
  if (!id3_write_file(ptrtag->year, (sizeof(ptrtag->year)-1), fp, fn))
    return FALSE;
  if (!id3_write_file(ptrtag->comment, (sizeof(ptrtag->comment)-1), fp, fn))
    return FALSE;
/*    fwrite(ptrtag->genre, 1, 1, fp);*/

  if (fputc(ptrtag->genre, fp) == EOF)
  {
    fclose(fp);
    print_error("write_tag: Error writing to %s", fn);
    return FALSE;
  }

  fclose(fp);
  return TRUE;
}


int
read_tag (FILE *fp, char *fn)
{
  if (!id3_read_file(ptrtag->songname, (sizeof(ptrtag->songname)-1), fp, fn))
    return FALSE;
  if (!id3_read_file(ptrtag->artist, (sizeof(ptrtag->artist)-1), fp, fn))
    return FALSE;
  if (!id3_read_file(ptrtag->album, (sizeof(ptrtag->album)-1), fp, fn))
    return FALSE;
  if (!id3_read_file(ptrtag->year, (sizeof(ptrtag->year)-1), fp, fn))
    return FALSE;
  if (!id3_read_file(ptrtag->comment, (sizeof(ptrtag->comment)-1), fp, fn))
    return FALSE;

/*  fread(&ptrtag->genre, 1, 1, fp); */
  ptrtag->genre = fgetc(fp);

  if (ptrtag->genre == EOF)
  {
    fclose(fp);
    print_error("tag_file: Error reading %s", fn);
    return FALSE;
  }

  return TRUE;
}


int
get_tag_string (int size, char *def_string, char *string, char *desc)
{

  printf("%s (%d chars): ", desc, (size-1));

  if (def_string != NULL && strlen(def_string) > 0)
  {
    strcpy(string, def_string);
    printf("%s\n", string);
    return TRUE;
  }

  if (flags.edit_tag && string != NULL && strlen(string) > 0)
  {
    printf("%s\n", string);
    return TRUE;
  }

  fgets(string, size, stdin);

  if (string[strlen(string)-1] == '\n')
    string[strlen(string)-1] = '\0';
  else
    while (getc(stdin) != '\n');

  return TRUE;
}


int
ask_tag (char *fn)
{

  get_tag_string(sizeof(ptrtag->songname), def_song, ptrtag->songname, "Song Name");
  get_tag_string(sizeof(ptrtag->artist), def_artist, ptrtag->artist, "Artist Name");

  if (flags.no_album == FALSE)
    get_tag_string(sizeof(ptrtag->album), def_album, ptrtag->album, "Album Name");

  if (flags.no_year == FALSE)
    get_tag_string(sizeof(ptrtag->year), def_year, ptrtag->year, "Year");

  if (flags.no_comment == FALSE)
    get_tag_string(sizeof(ptrtag->comment), def_comment, ptrtag->comment, "Comment");

  if (flags.no_genre == FALSE)
    get_tag_genre(&ptrtag->genre, def_genre);

  if (strlen(ptrtag->songname) < 1 || strlen(ptrtag->artist) < 1)
/*      || ptrtag->genre < 0 || ptrtag->genre >= genre_count) */
  {
    user_message(FALSE, "Not enough info entered to tag %s\n", fn);
    return FALSE;
  }

  return TRUE;
}


int
tag_file (char *fn)
{
  FILE *fp;
  long sizelesstag;
  short found_tag;

  if (id3_open_file(&fp, fn, "rb") == FALSE)
    return FALSE;

  ptrtag->tag[0] = '\0';
  ptrtag->songname[0] = '\0';
  ptrtag->artist[0] = '\0';
  ptrtag->album[0] = '\0';
  ptrtag->year[0] = '\0';
  ptrtag->comment[0] = '\0';
  ptrtag->genre = -1;

  if (id3_seek_header(fp, fn) == FALSE) return FALSE;
  sizelesstag = ftell(fp);

  if (!id3_read_file(ptrtag->tag, (sizeof(ptrtag->tag)-1), fp, fn))
    return FALSE;
/*
fread(&ptrtag->tag, (sizeof(ptrtag->tag)-1), 1, fp);
*/

  if (strcmp(ptrtag->tag, "TAG") != 0)
    found_tag = FALSE;
  else
    found_tag = TRUE;
    
  if (found_tag == FALSE)
  {
    fclose(fp);

    user_message(FALSE, "*** No ID3 tag found in %s\n", fn);

    if (flags.strip_tag == TRUE)
      return TRUE;

    /* return FALSE so no renaming is performed on files without a tag */
    if (flags.no_tag_prompt == TRUE)
      return FALSE;

    printf("\n===> Entering new tag info for %s:\n", fn);
    if (ask_tag(fn) == FALSE)
      return FALSE;

    if(write_tag(TRUE, fn) == FALSE)
      return FALSE;

    if (flags.tag_only == TRUE)
      return TRUE;

    if (id3_open_file(&fp, fn, "rb") == FALSE)
      return FALSE;

    if (id3_seek_header(fp, fn) == FALSE) return FALSE;

    if (!id3_read_file(ptrtag->tag, (sizeof(ptrtag->tag)-1), fp, fn))
      return FALSE;
  }  /*** Found a tag ****/
  else if (flags.force_tag == TRUE)    /* Always ask for a tag enabled? */
  {
    if (read_tag(fp, fn) == FALSE)
      return FALSE;

    fclose(fp);
    printf("\n===> Changing old tag info for %s:\n", fn);

    if (ask_tag(fn) == FALSE)
      return FALSE;

    if (write_tag(FALSE, fn) == FALSE)
      return FALSE;

    if (flags.tag_only == TRUE)
      return TRUE;

    if (id3_open_file(&fp, fn, "rb") == FALSE)
      return FALSE;

    if (id3_seek_header(fp, fn) == FALSE) return FALSE;

    if (!id3_read_file(ptrtag->tag, (sizeof(ptrtag->tag)-1), fp, fn))
      return FALSE;
  }
  else if (flags.tag_only == TRUE)
  {
    fclose(fp);
    user_message(FALSE, "===> Already has a tag: %s\n", fn);
    return TRUE;
  }
  else if (flags.strip_tag == TRUE)
  {
    fclose(fp);
    if (strip_tag(sizelesstag, fn) == FALSE)
      return FALSE;
    user_message(FALSE, "===> Removed ID3 tag from %s\n", fn);
    return TRUE;
  }

  if (read_tag(fp, fn) == FALSE)
    return FALSE;

  resize_tag_field(ptrtag->songname);
  resize_tag_field(ptrtag->artist);
  resize_tag_field(ptrtag->album);
  resize_tag_field(ptrtag->year);
  resize_tag_field(ptrtag->comment);
  fclose(fp);
  return TRUE;
}


void
show_tag (ID3_tag *tag, char *fn)
{
  user_message(FALSE, "===> %s:\n", fn);

  if (strlen(tag->songname) > 0)
    user_message(FALSE, "Song Name: %s\n", tag->songname);

  if (strlen(tag->artist) > 0)
    user_message(FALSE, "   Artist: %s\n", tag->artist);

  if (strlen(tag->album) > 0)
    user_message(FALSE, "    Album: %s\n", tag->album);

  if (strlen(tag->year) > 0)
    user_message(FALSE, "     Year: %s\n", tag->year);

  if (strlen(tag->comment) > 0)
    user_message(FALSE, "  Comment: %s\n", tag->comment);

  if (tag->genre >= 0 && tag->genre < genre_count)
    user_message(FALSE, "    Genre: %s\n", genre_table[tag->genre]);

  user_message(FALSE, "\n");
}
