
void
ll_swap_nodes(mp3_node *node1, mp3_node *node2)
{
  mp3_node *par1;

  par1 = node1->next;
  node1->next = node2->next;
  node2->next = par1;

  par1 = node1->prev;
  node1->prev = node2->prev;
  node2->prev = par1;

  if (node1->next == node1)
    node1->next = node2;
  if (node2->next == node2)
    node2->next = node1;

  if (node1->next)
    node1->next->prev = node1;
  if (node2->next)
    node2->next->prev = node2;
  if (node1->prev)
    node1->prev->next = node1;
  if (node2->prev)
    node2->prev->next = node2;

  return;
}


void
ll_sort (mp3_list list)
{
  mp3_node *i, *j;

  for (i = list; i != NULL; i = i->next)
  {
      for (j = i->next; j != NULL; j = j->next)
      {
          if (strcmp(i->id3.artist, j->id3.artist) > 0)
              ll_swap_nodes(i, j);
      }
  }
}







/*
 * llist.c
 *
 * These functions all deal with our list code.  We use a doubly linked
 * list that was implemented by me without knowing how to do it properly,
 * so you can expect to see a fair amount of horrible mistakes.
 *
 * Swapping/sorting still doesn't work, by the way.
 */

/*
void
ll_swap_nodes (mp3_node* node1, mp3_node* node2)
{
  mp3_node tmp1, tmp2;


  tmp1.next = node1->next;
  tmp1.prev = node1->prev;

  tmp2.next = node2->next;
  tmp2.prev = node2->prev;


  if (NULL != node1->prev)
      node1->prev->next = node2;
  if (NULL != node1->next)
      node1->next->prev = node2;


  if (NULL != tmp2.prev)
      tmp2.prev->next = node1;
  if (NULL != tmp2.next)
      tmp2.next->prev = node1;

  node1->next = tmp2.next;
  node1->prev = tmp2.prev;

  node2->next = tmp1.next;
  node2->prev = tmp1.prev;
}
*/



/*
 * ll_delete_node()
 *
 * Removes a node from the linked list by free'ing any allocated memory and
 * resetting `next' and `prev' so that the node cannot be accessed.
 *
 * Does not return a value.
 */
void
ll_delete_node (mp3_node* node)
{
  free (node->filename);

  if (NULL != node->prev)
      node->prev->next = node->next;
  if (NULL != node->next)
      node->next->prev = node->prev;

  free (node);

  return;
}




/*
 * ll_add_node()
 *
 * Adds a node to a list and reads all relevant information into the struct.
 *
 * Returns a pointer to the start of the list.
 */
mp3_list
ll_add_node (mp3_list list, const char* filename)
{
  FILE* tmp_file;
  mp3_list current = NULL;


  tmp_file = W_fopen (filename, "rb");
/*  if (!mp3_is_valid (tmp_file))*/
  if ( mp3_find_header( tmp_file ) < 0 )
      return list;


  if (NULL == list)
  {
      /* List is empty. */
      list = W_malloc (sizeof (mp3_node));
      list->next = NULL;
      list->prev = NULL;
      current = list;
  }
  else
  {
      current = ll_last_node (list);

      /* Create new node and attach to end of list. */
      current->next = W_malloc (sizeof (mp3_node));
      current->next->prev = current;
      current = current->next;
      current->next = NULL;
  }

  /* add all the file crap */
  current->filename = W_malloc (strlen (filename) + 1);
  strcpy (current->filename, filename);
  current->has_id3 = id3_check (tmp_file);
  current->id3 = id3_read (tmp_file);
  current->mp3 = mp3_read_header (tmp_file);
  current->file_length = flen (tmp_file);
  fclose (tmp_file);

  /* Take out a leading "the " if necessary */
  if (remove_leading_the)
      remove_leading_string(current->id3.artist, "the ");

  /* update globals */
  totalfiles++;
  totalfilesize += current->file_length;
  totalseconds += current->mp3.seconds;

  return list;
}



/*
 * ll_last_node()
 *
 * Returns a pointer to the last node in the list.
 */
mp3_node*
ll_last_node (mp3_list list)
{
  mp3_node* temp;

  for (temp = list; NULL != temp->next; temp = temp->next);
  return temp;
}




/*
 * ll_first_node()
 *
 * Returns a pointer to the last node in the list.
 */
mp3_node*
ll_first_node (mp3_list list)
{
  mp3_node* temp;

  for (temp = list; NULL != temp->prev; temp = temp->prev);
  return temp;
}




/*
 * ll_delete_list()
 *
 * Frees an entire linked list.
 */
void
ll_delete_list (mp3_list list)
{
  mp3_node* current;

  if (NULL == list)
      return;

  current = ll_first_node (list);

  if (NULL == current->next)
      return;
  else
      current = current->next;

  while (current->next != NULL)
  {
      ll_delete_node (current->prev);
      current = current->next;
  }

  ll_delete_node (current);

  return;
}
/*EOF*/
