/*
 * mp3html.c    by Ben Canning <bdc@bdc.cx>
 *
 * For information about using/installing MP3html, see the file 'README'.
 *
 * This is the main code for MP3html.  In fact, the only function in here is
 * 'main()', which only serves to parse the commandline and then call the
 * functions which do the actual work.  Unfortunately for you, there is very
 * little commenting in here, so it might be a bit diffcult to understand
 * exactly WTF I was thinking in a few places.  Enjoy!
 */


#include "include.c"    /* Tells which files to include */



int
main (int argc, char **argv)
{
  char*    header = NULL;
  char*    footer = NULL;
  char*    middle = NULL;
  int      current_option = 0, *option_index = 0;
  FILE*    outfile = stdout;
  mp3_list mlist = NULL;
  mp3_list temp = NULL;


  /* Necessary so we don't try to substitute NULL for tags later on */
  unknown_text = W_realloc (unknown_text, 1);
  unknown_text[0] = 0;


  /* If there ain't enough commandline parameters, let 'em know! */
  if (1 == argc)
  {
      printf ("%s %s (%s) by %s <%s>\n", NAME, VERSION, DATE, AUTHOR, EMAIL);
      printf ("usage: %s %s\n", argv[0], USAGE);
      printf ("try \"mp3html --help\" for more information\n");
      return 0;
  }


  /* Parse the commandline */
  while( -1 != current_option )
  {
      current_option = getopt_long( argc, argv, "H:F:h:f:t:r:p:D:u:o:i:vS",
                                    CMDLINE_OPTIONS, option_index );

      switch( current_option )
      {
          case '1': show_commandline_help( argv[0] ); break;
          case '2': show_version(); break;
          case '3': remove_leading_the = 1; break;
          case '4': stereo_text = optarg; break;
          case '5': mono_text = optarg; break;
          case 'o': outfile = W_fopen( optarg, "wb" ); break;
          case 'S': underscores_for_spaces = 1; break;
          case 't': middle = optarg; break;
          case 'H': header = load_template (optarg, header); break;
          case 'F': footer = load_template (optarg, footer); break;
          case 'h': header = optarg; break;
          case 'f': footer = optarg; break;
          case 'r': mlist = recurse_directory (mlist, optarg); break;
          case 'p': mlist = load_playlist (mlist, optarg); break;
          case 'D': macro_add( optarg ); break;
          case 'i': increment_base = atoi( optarg ); break;
          case 'u': optarg[29] = 0; unknown_text = optarg; break;
          case -1 :
          default : break;
      }
  }

  if( NULL == middle && optind != argc)
      middle = load_template( argv[optind++], middle);

  /* Still no middle template?  Well, we can't continue.... */
  if (NULL == middle)
      exit_message ("No middle template was defined.", NULL, 10);


  for( current_option = optind; current_option < argc; current_option++)
      mlist = ll_add_node( mlist, argv[current_option] );

  /* No MP3s?  Heck, don't expect us to function now! */
  if (NULL == mlist)
      exit_message ("You didn't put any MP3s on the cmdline.", NULL, 10);


  /* Show header */
  show_static_template (header, outfile);


/*  ll_sort (mlist); */
  /* Go through and run the files through the middle template */
  for( temp = mlist; temp != NULL; temp = temp->next )
      fprintf( outfile, substitute_tags( temp, middle ) );


  /* Show footer */
  show_static_template (footer, outfile);


  /* Clean up - free the list */
  ll_delete_list (mlist);

  return 0;
}
/*EOF*/
