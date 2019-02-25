/**
 * Function to draw a table on the psdump user interface.
 *
 * @author Magnus Henoch <mange@freemail.hu>
 */

/*
 * Copyright (C) 2002, Magnus Henoch <mange@freemail.hu>
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

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * Draws a table, or a table cell, and recursively render the contents
 * of the table cells. 
 *
 * @param partp A pointer to the layout part representing the line.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_draw_table(struct layout_part *partp)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
  uint32_t colour1, colour2;
/*    struct layout_part *rowp, *cellp; */
  int border = 0;

  /*
  fprintf(stderr, "%s: p_x=%d, p_y=%d, p_w=%d, p_h=%d, g_w=%d, g_h=%d\n",
	  __FUNCTION__, partp->geometry.x_position, partp->geometry.y_position,
	  partp->geometry.width, partp->geometry.height, 
	  geometry.width, geometry.height);
  */

  /* If the table has a background colour worth mentioning, we fill the area
   * with that colour here. Also select colour for the border.
   */
  colour1 = 0xc0c0c0;
  colour2 = 0x000000;
  if(partp->type == LAYOUT_PART_TABLE) {
    border = partp->data.table.border;
  } else if (partp->type == LAYOUT_PART_TABLE_CELL) {
#ifdef DEBUG
    fprintf(stderr, "about to draw table cell background (%x)\n", partp->data.table_cell.background_colour);
#endif
    colour1 = 0x000000;
    colour2 = 0xc0c0c0;
    if(!(partp->data.table_cell.background_colour & 0x80000000)) {
#ifdef DEBUG
      fprintf(stderr, "really doing it\n");
#endif
      psdump_set_color(partp->data.table_cell.background_colour);
      fprintf(info->outfile,
	      "%d %d %d %d fillrectangle\n",
	      psdump_translate_x(partp->geometry.x_position), psdump_translate_y(partp->geometry.y_position),
	      psdump_translate_x(partp->geometry.x_position + partp->geometry.width),
	      psdump_translate_x(partp->geometry.y_position + partp->geometry.height));
    }

    if(partp->parent && partp->parent->parent &&
       partp->parent->parent->type == LAYOUT_PART_TABLE)
      border = partp->parent->parent->data.table.border;
  }

  if(border > 0) {
    int x1, y1, x2, y2;

    x1 = psdump_translate_x(partp->geometry.x_position);
    y1 = psdump_translate_y(partp->geometry.y_position);
    x2 = psdump_translate_x(partp->geometry.x_position + partp->geometry.width - 2);
    y2 = psdump_translate_y(partp->geometry.y_position + partp->geometry.height - 1);
    psdump_set_color(colour1);
    /* drawline is defined in the prolog, in psdump_render */
    fprintf(info->outfile,
	    "%d %d %d %d drawline\n",
	    x1, y1,
	    x2, y1);
    fprintf(info->outfile,
	    "%d %d %d %d drawline\n",
	    x1, y1,
	    x1, y2);
    psdump_set_color(colour2);
    fprintf(info->outfile,
	    "%d %d %d %d drawline\n",
	    x1, y2,
	    x2, y2);
    fprintf(info->outfile,
	    "%d %d %d %d drawline\n",
	    x2, y1,
	    x2, y2);
  }
#if 0
  if(partp->type == LAYOUT_PART_TABLE) {
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
#if 0
	if(!(cellp->geometry.x_position - scroll.x_position > 
	     geometry.x_position + geometry.width ||
	     cellp->geometry.y_position - scroll.y_position > 
	     geometry.y_position + geometry.height ||
	     cellp->geometry.x_position - scroll.x_position + 
	     cellp->geometry.width < geometry.x_position ||
	     cellp->geometry.y_position - scroll.y_position + 
	     cellp->geometry.height < geometry.y_position)) {
#endif
	  
	  /* Draw the frame around the cell. */
	  psdump_draw_table(cellp);

	  /* Render the parts placed inside the table cell. */
	  if(cellp->child != NULL) {
	    psdump_render(cellp);
	  }
#if 0
	}
#endif
	cellp = cellp->next;
      }

      rowp = rowp->next;
    }
  }
#endif
  return 0;
}

