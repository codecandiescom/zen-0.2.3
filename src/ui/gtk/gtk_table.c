/**
 * Function to draw a table on the GTK+ user interface.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

/*
 * Copyright (C) 1999, Tomas Berndtsson <tomas@nocrew.org>
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
#include <gtk/gtk.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * Sets up a table and the table cells with widgets, so that they are
 * drawn properly by the event handlers.
 *
 * @param partp A pointer to the layout part representing the line.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_table(struct layout_part *partp, GtkLayout *display)
{
  struct layout_part *rowp, *cellp;
  struct layout_rectangle rect;
  uint32_t *colourp;
  uint32_t old_background_colour;

  /* Draw the edge of the whole table. */
  if(partp->data.table.border)
    gtkui_draw_rectangle(partp->geometry, display, 0xffffff, 0x000000);

  rowp = partp->child;
  while(rowp) {
    if(rowp->type != LAYOUT_PART_TABLE_ROW) {
      rowp = rowp->next;
      continue;
    }

    cellp = rowp->child;
    while(cellp) {
      if(cellp->type != LAYOUT_PART_TABLE_CELL) {
	cellp = cellp->next;
	continue;
      }

      if(cellp->child != NULL) {
	/* Draw the outlines of the table cell. */
	rect = cellp->geometry;

	if(partp->data.table.border) {
	  gtkui_draw_rectangle(rect, display, 0x000000, 0xffffff);
	
	  rect.x_position++;
	  rect.y_position++;
	  rect.width -= 2;
	  rect.height -= 2;
	}

	/* Render the parts placed inside the table cell. We also save the
	 * old background colour, and restore it after rendering the contents
	 * of this table cell.
	 */
	colourp = gtk_object_get_data(GTK_OBJECT(display), 
				      "background_colour");
	old_background_colour = *colourp;
	if(!(cellp->data.table_cell.background_colour & 0x80000000))
	  *colourp = cellp->data.table_cell.background_colour;
	  
	gtkui_fill_area(rect, display, *colourp);
	  
	gtkui_render(cellp->child, display);
	  
	*colourp = old_background_colour;
      }

      cellp = cellp->next;
    }

    rowp = rowp->next;
  }

  return 0;
}


