#ifndef FOPEN_ERROR
  #define FOPEN_ERROR  102
#endif

#ifndef MALLOC_ERROR
    #define MALLOC_ERROR 103
#endif

#ifndef REALLOC_ERROR
    #define REALLOC_ERROR 101
#endif


/*
  W_fopen()
  --
  A simple wrapper for fopen so I don't need to keep putting in all that
  annoying error-checking code.  Returns a FILE* handle.
*/

FILE *
W_fopen (const char *filename, const char *mode)
{
  FILE *f;

  f = fopen (filename, mode);

  if (!f)
  {
      fprintf (stderr,
          "\n"
          "    Hal, open the file\n"
          "    Hal, open the damn file, Hal\n"
          "    open the, please Hal\n"
          "\n"
          "Bad news... W_fopen (the fopen wrapper) seems to think that it\n"
          "can't open \"%s\". You can bitch at it, but I doubt it'll help.\n"
          , filename);
      fprintf (stderr, "\nW_fopen (\"%s\", \"%s\");\n\n", filename, mode);

      exit (FOPEN_ERROR);
  }

  return (f);

  return (NULL);
}




void *
W_malloc (size_t size)
{
  register void *value;

  value = malloc (size);

  if (!value)
  {
    fprintf (stderr,
        "\n"
        "    I'm sorry, there's -- um --\n"
        "    insufficient -- what's-it-called?\n"
        "    The term eludes me ...\n"
        "\n"
        "Bad news... W_malloc (the malloc wrapper) is under the impression\n"
        "that there is no more memory to allocate.  Needless to say, this\n"
        "would result in very Bad Things(tm) in our program.  For that\n"
        "reason, we're terminating with extreme prejudice.\n");

    exit (MALLOC_ERROR);
  }

  return value;
}


void*
W_realloc (void *ptr, size_t size)
{
  register void *value;

  value = realloc (ptr, size);

  if (NULL == value)
  {
    fprintf (stderr,
        "\n"
        "    I'm sorry, there's -- um --\n"
        "    insufficient -- what's-it-called?\n"
        "    The term eludes me ...\n"
        "\n"
        "Bad news... W_realloc (the realloc wrapper) is under the impression\n"
        "that there is no more memory to allocate.  Needless to say, this\n"
        "would result in very Bad Things(tm) in our program.  For that\n"
        "reason, we're terminating with extreme prejudice.\n");

    exit (REALLOC_ERROR);
  }

  return value;
}
/*EOF*/
