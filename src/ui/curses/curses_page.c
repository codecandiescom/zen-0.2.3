/**
 * Code that handles all the page data and does the page rendering.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include "info.h"

/**
 * Add a link to the page.
 * 
 * @param page Page to add link to.
 * @param partp Part that points to object that is the link.
 * @param linkp Part that contains link information.
 */
static void add_link(struct curses_page *this, 
		     struct layout_part *partp,
		     struct layout_part *linkp)
{
  struct curses_link *new, *lastp;
   
  assert(this && partp && linkp);
  
  new = (struct curses_link *)malloc(sizeof(struct curses_link));
  if(new == NULL)
    return;

  new->link = partp->geometry;
  new->part = linkp;
  new->visual_part = partp;

  if(this->links == NULL) {
    new->next = NULL;
    new->previous = NULL;
    links = new;
  } else {
    /* Find the last element in the list of links. */
    lastp = links;
    while(lastp->next)
      lastp = lastp->next;

    /* Put the link last in the list. */
    new->next = NULL;
    new->previous = lastp;
    lastp->next = new;
  }  
}

/**
 * Deletes all the links (suprised?)
 * 
 * @param page Page to delete links of.
 */
static void delete_all_links(struct curses_page *this)
{
  struct curses_link *linkp, *tmp;

  assert(this);
   
  linkp = this->links;
  while(linkp) {
    tmp = linkp;
    linkp = linkp->next;
    free(tmp);
  }

  links = NULL;
}
   
/**
 * create a list of links for the page.
 * 
 * @param this Page to create links for.
 * @param parts Parts to work on.
 * 
 * @return 0 on success and 1 on failure.
 */
static int create_links(struct curses_page *this,
			struct layout_part *parts)
{
  struct layout_part *partp;
  static struct layout_part *linkp = NULL;

  assert(this && parts);
   
  partp = parts;

  if(partp && partp->type == LAYOUT_PART_PAGE_INFORMATION) {
    delete_all_links(this);
  }  

  while(partp) {
    switch(partp->type) {
    case LAYOUT_PART_TEXT:
      if(linkp)
	add_link(partp, linkp);
      break;
    
    case LAYOUT_PART_LINK:
      {
	struct layout_part *old_linkp;

	old_linkp = linkp;
	linkp = partp;
	create_links(this, partp->child);
	linkp = old_linkp;
      }
      break;
       
    default:
    }

    create_links(this, partp->child);
    if(partp->type == LAYOUT_PART_PAGE_INFORMATION)
      break;

    partp = partp->next;
  }
}

/**
 * This creates a new page description.
 * 
 * @param parts The layout part for the page.
 * @return Returns A new page on success and NULL otherwise.
 */
struct curses_page *curses_page_new(struct layout_parts *parts)
{
   struct curses_page *page =
     (struct curses_page*) malloc(sizeof(struct curses_page));
   
   if (!page) {
     fprintf(stderr, "Error: Cannot allocate a new page:\n");
     return NULL;
   }
   
   page->parts = parts;
   page->next = page->previous = NULL;
   page->scroll.x_position = 0;
   page->scroll.y_position = 0;
   page->geometry.x_position = 0;
   page->geometry.y_position = 0;
   
   /* We need to beable to make an anchor page, with out parts */
   if (!parts) return page;
   
   if (create_links(page, NULL)) {
     fprintf(stderr, "Error: Could not create links for page:\n");
     free(page);
     return NULL;
   }
   
   page->activelink = page->links;
   
   return page;
}

/**
 * Links a page onto the end of a list of pages.
 * 
 * @param page Page to insert the inductee after.
 * @param inductee Page to insert.
 * 
 * @return Returns the address of the newly inducted page.
 */
struct curses_interface *curses_page_linkin(struct curses_page *this,
 		                            struct curses_page *inductee)
{
   assert(this);
   
   if (!inductee) {
      fprintf(stderr, "Error: There is no page to linkin:\n");
      return NULL;
   }
   
   for (; this->next; this = this->next)
     /* Do nothing */;
   
   inductee->next = NULL;
   this->next = inductee;
   inductee->previous = this;
   
   return inductee;
}

/**
 * Moves to the next link on the page.
 * 
 * @param page Page to work on.
 */
void curses_page_nextlink(struct curses_page *this)
{
   assert(this);
   if (this->activelink->next)
     this->activelink = this->activelink->next;
}

/**
 * Moves to the previous link on the page.
 * 
 * @param page Page to work on.
 */
void curses_page_previouslink(struct curses_page *this)
{
   assert(this);
   if (this->activelink->previous)
     this->activelink = this->activelink->previous;
}

/**
 * Scrolls the page around.
 * 
 * @param page Page to scroll.
 * @param x_offset Scroll x axis by this much (unimplemented).
 * @param y_offset Scroll y axis by this much.
 * 
 * @return Returns 1 on failure, 0 on success.
 */
int curses_page_scroll(struct curses_page *this,
		       int x_offset, int y_offset)
{
   assert(this);
   return 0;
}

/**
 * Renders the page all pretty like.
 * 
 * @param page Page to render.
 * @param window Window to render it on.
 * 
 * @return Returns 1 on failure and 0 on success.
 */
int curses_page_render(struct curses_page *this,
		       struct curses_window *window)
{
   assert(this && window);
   return 0;
}

/**
 * Deletes a page.
 * 
 * @param page Page to delete.
 */
static void page_delete(struct curses_page *this)
{
   assert(this);
   
   this->previous->next = this->next;
   this->next->previous = this->previous;
   delete_all_links(this->links);
   free(this);
}

/**
 * Deletes all the pages. Finds start of list and deletes to the end.
 * 
 * @param this List of pages to delete.
 */
void curses_page_deleteall(struct curses_page *this) 
{
   assert(this);
   
   for (; this->previous; this = this->previous);
   for (this = this->next; this && this->next;
	this = this->next)
     page_delete(this->previous);
   
   page_delete(this->previous);
}
