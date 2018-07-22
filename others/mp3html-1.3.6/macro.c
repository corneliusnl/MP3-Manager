/*
 * macro.c
 *
 * This file contains a few functions used to define your own macros and
 * their values.  It stores them in two global variables and uses one more
 * for keeping track of how many macros have been defined.
 */


/* Macro globals */
  int    macro_num = 0;
  char** macro_name = NULL;
  char** macro_data = NULL;



/*
 * macro_add()
 *
 * Adds a macro name and data to the arrays.  This takes care of all memory
 * allocation for you.
 */
void
macro_add( char* macro )
{
  char* name, * data;

  /* If there's no '=' in macro, terminate */
  if( NULL == strchr( macro, '=' ) )
      return;

  name = macro;
  data = strchr( macro, '=' ) + 1;
  data[-1] = 0;

  macro_num++;

  macro_name = W_realloc( macro_name, sizeof(char*) * macro_num);
  macro_data = W_realloc( macro_data, sizeof(char*) * macro_num);

  macro_name[ macro_num - 1 ] = W_malloc( strlen( name ) + 1 );
  macro_data[ macro_num - 1 ] = W_malloc( strlen( data ) + 1 );

  strcpy( macro_name[ macro_num - 1 ], name );
  strcpy( macro_data[ macro_num - 1 ], data );

  return;
}




/*
 * macro_free()
 *
 * Goes through and frees all the allocated memory for the macro arrays.
 * Call this at shutdown to clean up.
 */
void
macro_free( void )
{
  int loop1 = 0;

  if (macro_num == 0)
      return;

  for (loop1 = 0; loop1 < macro_num; loop1++)
  {
      free( macro_name[loop1] );
      free( macro_data[loop1] );
  }

  free( macro_name );
  free( macro_data );

  return;
}
/*EOF*/
