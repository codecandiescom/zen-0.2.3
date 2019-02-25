/**
 * Function to draw a horizontal line on the Postscript dump
 * user interface.
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

/**
 * Draws a simple horizontal line as wide as specified.
 *
 * @param partp A pointer to the layout part representing the line.
 * @param geometry The current geometry of the display, describing what
 * @param geometry part and where to render the line.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_draw_line(struct layout_part *partp)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
  uint32_t colour1, colour2;
  struct layout_rectangle rect;

  colour1 = 0x666666;
  colour2 = 0xeeeeee;

  /* Calculate the rectangle to represent the line. */
  rect.x_position = partp->geometry.x_position + 8;
  rect.y_position = partp->geometry.y_position  +
    (partp->geometry.height - partp->data.line.size) / 2;
  rect.width = partp->geometry.width - 16;
  rect.height = partp->data.line.size;

  fprintf(info->outfile,"newpath\n");
  psdump_set_color(colour1);
  fprintf(info->outfile,
	  "%d %d moveto\n"
	  "%d %d lineto\n"
	  "stroke\n",
	  psdump_translate_x(rect.x_position),psdump_translate_y(rect.y_position),
	  psdump_translate_x(rect.x_position + rect.width - 2),psdump_translate_y(rect.y_position));
#if 0
  FBhline(fb, 
	  rect.x_position,
	  rect.x_position + rect.width - 2,
	  rect.y_position,
	  colour1);
#endif

  fprintf(info->outfile,"newpath\n");
  psdump_set_color(colour1);
  fprintf(info->outfile,
	  "%d %d moveto\n"
	  "%d %d lineto\n"
	  "stroke\n",
	  psdump_translate_x(rect.x_position),psdump_translate_y(rect.y_position),
	  psdump_translate_x(rect.x_position),psdump_translate_y(rect.y_position + rect.height - 1));
#if 0
  FBline(fb, 
	 rect.x_position,
	 rect.y_position,
	 rect.x_position,
	 rect.y_position + rect.height - 1,
	 colour1);
#endif

  fprintf(info->outfile,"newpath\n");
  psdump_set_color(colour1);
  fprintf(info->outfile,
	  "%d %d moveto\n"
	  "%d %d lineto\n"
	  "stroke\n",
	  psdump_translate_x(rect.x_position + 1),psdump_translate_y(rect.y_position + rect.height - 1),
	  psdump_translate_x(rect.x_position + rect.width - 2),psdump_translate_y(rect.y_position + rect.height - 1));
#if 0
  FBhline(fb, 
	  rect.x_position + 1,
	  rect.x_position + rect.width - 2,
	  rect.y_position + rect.height - 1,
	  colour2);
#endif

  fprintf(info->outfile,"newpath\n");
  psdump_set_color(colour1);
  fprintf(info->outfile,
	  "%d %d moveto\n"
	  "%d %d lineto\n"
	  "stroke\n",
	  psdump_translate_x(rect.x_position + rect.width - 2),psdump_translate_y(rect.y_position + 1),
	  psdump_translate_x(rect.x_position + rect.width - 2),psdump_translate_y(rect.y_position + rect.height - 1));
#if 0
  FBline(fb, 
	 rect.x_position + rect.width - 2,
	 rect.y_position + 1,
	 rect.x_position + rect.width - 2,
	 rect.y_position + rect.height - 1,
	 colour2);
#endif

  return 0;
}

