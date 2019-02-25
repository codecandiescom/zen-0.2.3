/**
 * Functions to render parts of a page in the Postscript dump user
 * interface.
 *
 * @author Magnus Henoch <mange@freemail.hu>
 */

/*
 * Copyright (C) 1999, Magnus Henoch <mange@freemail.hu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "zen_ui.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#if 0
/* The linked list of links. */
struct ofbis_link *links = NULL;

/**
 * Adds a link to the linked list of links.
 *
 * @param partp A pointer to a layout part representing the object 
 * @param partp which is shown as a link.
 * @param linkp A pointer to the layout part containing the link information.
 */
static void add_link(struct layout_part *partp, struct layout_part *linkp)
{
  struct ofbis_link *new, *lastp;
  
  new = (struct ofbis_link *)malloc(sizeof(struct ofbis_link));
  if(new == NULL)
    return;

  new->link = partp->geometry;
  new->part = linkp;
  new->visual_part = partp;

  if(links == NULL) {
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
 * Delete all previous links stored on the linked list of links.
 */
static void delete_all_links(void)
{
  struct ofbis_link *linkp, *tmp;

  linkp = links;
  while(linkp) {
    tmp = linkp;
    linkp = linkp->next;
    free(tmp);
  }

  links = NULL;
}

/**
 * Create a list of links, containing all links on the whole page.
 *
 * @param parts A pointer to the first layout part in the list.
 */
void ofbis_create_links(struct layout_part *parts)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  struct layout_part *partp;
  static struct layout_part *linkp = NULL;

  partp = parts;

  if(partp && partp->type == LAYOUT_PART_PAGE_INFORMATION) {
    delete_all_links();
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
	ofbis_create_links(partp->child);
	linkp = old_linkp;
      }
      break;

    case LAYOUT_PART_GRAPHICS:
      if(linkp) {
	add_link(partp, linkp);
	if(partp->data.graphics.border == 0)
	  partp->data.graphics.border = 1;
	partp->data.graphics.border_colour = info->default_link_colour;
      }
      break;

    default:
    }

    ofbis_create_links(partp->child);
    if(partp->type == LAYOUT_PART_PAGE_INFORMATION)
      break;

    partp = partp->next;
  }
}
#endif

/**
 * Render the page by calling the appropriate rendering functions.
 *
 * @param parts A pointer to the first layout part in the list to be rendered.
 * @param geometry The offset and size of the screen area to render on. This is
 * @param geometry relative to the full rendering area, not to the full screen.
 * @param scroll The scroll offset. Only the position values are used.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_render(struct layout_part *parts)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
  struct layout_part *partp;
/*   struct layout_rectangle full_geometry, control_geometry; */

/* So this is it. We are supposed to output Postscript to
 * info->outfile. Page size:
 *
 * Left margin: info->left_margin
 * Bottom margin: info->bottom_margin
 * Height: psdump_ui->ui_display->height
 * Width: psdump_ui->ui_display->width
 *
 * Note that Y coordinates start at the bottom of the page,
 * so they will have to be reverted, in psdump_translate_y.
 */

  partp = parts;

  while(partp) {

    /*
    fprintf(stderr, 
	    "geo.x=%d, geo.y=%d, geo.w=%d, geo.h=%d\n",
	    geometry.x_position, geometry.y_position,
	    geometry.width, geometry.height);

    fprintf(stderr, 
	    "scroll.x=%d, scroll.y=%d\n",
	    scroll.x_position, scroll.y_position);
	    
    fprintf(stderr, 
	    "%s: partp->type=%d, partp->child=%p, partp->x=%d, partp->y=%d, partp->w=%d, partp->h=%d\n",
	    __FUNCTION__, partp->type, partp->child, 
	    partp->geometry.x_position, partp->geometry.y_position,
	    partp->geometry.width, partp->geometry.height);
    */

    if (psdump_translate_y(partp->geometry.y_position + partp->geometry.height) < info->bottom_margin) {
      if (partp->type != LAYOUT_PART_TABLE && partp->type != LAYOUT_PART_PAGE_INFORMATION) {
	psdump_new_page(partp->geometry.y_position);
      }
    }

#if 0
    /* Check that the part will fit within the requested display. This
     * does not yet perform any clipping of the parts, so whole parts will
     * be left out, except for images, and tables.
     */
    if((partp->child == NULL ||
	partp->type == LAYOUT_PART_TABLE ||
	partp->type == LAYOUT_PART_TABLE_CELL) &&
       (partp->geometry.x_position - scroll.x_position > 
	geometry.x_position + geometry.width - 1 ||
	partp->geometry.y_position - scroll.y_position > 
	geometry.y_position + geometry.height - 1 ||
	partp->geometry.x_position - scroll.x_position + 
	partp->geometry.width - 1 < geometry.x_position ||
	partp->geometry.y_position - scroll.y_position + 
	partp->geometry.height - 1 < geometry.y_position)) {

      partp = partp->next;
      continue;
    } 

    /*     fprintf(stderr, "Passed one test.\n"); */

    if(partp->child == NULL &&
       partp->type != LAYOUT_PART_GRAPHICS &&
       (partp->geometry.x_position - scroll.x_position < 
	geometry.x_position ||
	partp->geometry.y_position - scroll.y_position < 
	geometry.y_position ||
	partp->geometry.x_position - scroll.x_position + 
	partp->geometry.width - 1 > 
	geometry.x_position + geometry.width - 1 ||
	partp->geometry.y_position - scroll.y_position + 
	partp->geometry.height - 1 > 
	geometry.y_position + geometry.height - 1)) {
      
      partp = partp->next;
      continue;
    }

    /*     fprintf(stderr, "Passed both tests.\n"); */
#endif

    /* Only the supported parts are represented here, all the others are
     * covered by the default statement, which does not do anything.
     */
    switch(partp->type) {
    case LAYOUT_PART_PAGE_INFORMATION:

/*       info->default_background_colour =  */
/* 	partp->data.page_information.background_colour; */
      info->default_text_colour = 
	partp->data.page_information.text_colour;
      info->default_link_colour = 
	partp->data.page_information.link_colour;
      info->default_active_link_colour = 
	partp->data.page_information.active_link_colour;
      info->default_visited_link_colour = 
	partp->data.page_information.visited_link_colour;
#if 0
      /* We cannot simply set the background colour to a new colour, since 
       * the images will be mapped to the original palette. Instead, we should
       * fill the screen with the colour. This is possible here, since we know
       * that this part is always the first one in the list.
       */
      full_geometry.x_position = info->x_position + geometry.x_position;
      full_geometry.y_position = info->y_position + geometry.y_position;
      full_geometry.width = geometry.width;
      full_geometry.height = geometry.height;
      ofbis_fill_area(full_geometry, info->default_background_colour);
      control_geometry.x_position = 0;
      control_geometry.y_position = 0;
      control_geometry.width = ofbis_ui->ui_display.width;
      control_geometry.height = info->y_position - 1;
      ofbis_fill_area(control_geometry, info->control_field_colour);
      ofbis_set_title(partp->data.page_information.title);
      ofbis_set_url(partp->data.page_information.url);
#endif

      fprintf(info->outfile, 
	      "%%!PS-Adobe-3.0\n"
	      "%%%%Pages: (atend)\n"
	      "%%%%PageOrder: Ascend\n"
	      "%%%%EndComments\n"
	      "%%%%BeginProlog\n"
	      "/drawline {newpath moveto lineto stroke} bind def\n"
	      "/pathrectangle {/Y2 exch def /X2 exch def /Y1 exch def /X1 exch def\n"
	      "    newpath X1 Y1 moveto X1 Y2 lineto X2 Y2 lineto X2 Y1 lineto closepath} def\n"
	      "/drawrectangle {pathrectangle stroke} def\n"
	      "/fillrectangle {pathrectangle fill} def\n"
	      "%%%%EndProlog\n"
	      );
      fprintf(info->outfile, "%%%%Page: %d %d\n"
                             "gsave\n", info->page_number, info->page_number);

      /* Print title and URL in some kind of header?
       * Print page number?
       */
      break;

    case LAYOUT_PART_TEXT:
      psdump_draw_text(partp);
      break;

    case LAYOUT_PART_GRAPHICS:
      psdump_draw_image(partp);
      break;

    case LAYOUT_PART_LINE:
      psdump_draw_line(partp);
      break;

    case LAYOUT_PART_TABLE:
    case LAYOUT_PART_TABLE_CELL:
      psdump_draw_table(partp);
      break;


    default:
      break;
    }

#if 0
    if(partp->type != LAYOUT_PART_TABLE)
#endif
      psdump_render(partp->child);
    if(partp->type == LAYOUT_PART_PAGE_INFORMATION)
      break;

    partp = partp->next;
  }

  return 0;
}

#if 0
/**
 * Render the given part in active link colour, and restore the previously
 * active link to its original colour.
 *
 * @param linkp A pointer to the link which is going to become the new 
 * @param linkp active link. If NULL, we only reinitialize the previously
 * @param linkp saved active link as NULL, to start over.
 * @param geometry The geometry of the page, so that the rendering functions
 * @param geometry will know where to draw the part. 
 */
void ofbis_activate_link(struct ofbis_link *linkp,
			 struct layout_rectangle geometry,
			 struct layout_rectangle scroll)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  static struct layout_part *previously_active = NULL;
  static uint32_t previous_colour = 0;
  struct layout_part *partp;

  if(linkp)
    partp = linkp->visual_part;
  else
    partp = NULL;

  /* If the previously active part is the same as the one we are trying to
   * activate now, we do not need to do anything, and therefore we will save
   * time and money by simply returning from the function.
   */
  if(previously_active == partp)
    return;

  /* Restoring the old part. */
  if(previously_active) {
    switch(previously_active->type) {
    case LAYOUT_PART_TEXT:
      previously_active->data.text.style.colour = previous_colour;
      ofbis_render(previously_active, geometry, scroll);      
      break;
      
    case LAYOUT_PART_GRAPHICS:
      previously_active->data.graphics.border_colour = previous_colour;
      ofbis_render(previously_active, geometry, scroll);
      break;
      
    default:
      fprintf(stderr, "If you see this, the programmer has made a mistake.\n");
    }
  }

  if(linkp == NULL) {
    previously_active = NULL;
    ofbis_set_status_text(NULL);
    return;
  }

  /* Make the current part a new colour. */
  switch(partp->type) {
  case LAYOUT_PART_TEXT:
    previous_colour = partp->data.text.style.colour;
    partp->data.text.style.colour = info->default_active_link_colour;
    ofbis_render(partp, geometry, scroll);
    break;

  case LAYOUT_PART_GRAPHICS:
    previous_colour = partp->data.graphics.border_colour;
    partp->data.graphics.border_colour = info->default_active_link_colour;
    ofbis_render(partp, geometry, scroll);
    break;

  default:
    fprintf(stderr, "If you see this, the programmer has made a mistake.\n");
  }

  ofbis_set_status_text(linkp->part->data.link.href);
  
  previously_active = partp;
}
#endif
