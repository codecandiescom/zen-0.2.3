/**
 * Function to draw an image on the Postscript dump user interface.
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
 * Draw an an empty box. Images to come.
 *
 * @param partp A pointer to the layout part representing the image.
 * @param geometry The current geometry of the display, which describes how
 * @param geometry and where to render the image.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_draw_image(struct layout_part *partp)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
  uint32_t colour;
  int x1, y1, x2, y2;

  x1 = psdump_translate_x(partp->geometry.x_position);
  y1 = psdump_translate_y(partp->geometry.y_position);
  x2 = psdump_translate_x(partp->geometry.x_position + partp->geometry.width);
  y2 = psdump_translate_y(partp->geometry.y_position + partp->geometry.height);


  /*
  fprintf(stderr, 
	  "geometry.x_position=%d, geometry.y_position=%d\n"
	  "geometry.width=%d, geometry.height=%d\n"
	  "partp->geometry.x_position=%d, partp->geometry.y_position=%d\n"
	  "partp->geometry.width=%d, partp->geometry.height=%d\n"
	  "x_position=%d, y_position=%d\n"
	  "cut.x=%d, cut.y=%d\n"
	  "cut.w=%d, cut.h=%d\n",
	  geometry.x_position, geometry.y_position,
	  geometry.width, geometry.height,
	  partp->geometry.x_position, partp->geometry.y_position,
	  partp->geometry.width, partp->geometry.height,
	  x_position, y_position,
	  cut.x_position, cut.y_position,
	  cut.width, cut.height);
  */

#if 0
  if(partp->data.graphics.data) {
    FBBLTPBLK *fbb;
    
    fbb = FBgetbltpblk(fb);
    
    fbb->b_wd = cut.width;
    fbb->b_ht = cut.height;
    fbb->plane_ct = ofbis_ui->ui_display.bit_depth;
    fbb->d_xmin = x_position + info->x_position;
    fbb->d_ymin = y_position + info->y_position;
    fbb->s_xmin = cut.x_position;
    fbb->s_ymin = cut.y_position;
    fbb->s_form = (unsigned short *)partp->data.graphics.data;
    fbb->s_nxwd = ofbis_ui->ui_display.bit_depth * 2;
    if(fb->finf.type == FB_TYPE_PACKED_PIXELS)
      fbb->s_nxln = partp->geometry.width * ofbis_ui->ui_display.bit_depth / 8;
    else
      fbb->s_nxln = (fbb->b_wd + 15) / 16;
    fbb->s_nxpl = 2;
    
    FBbitblt(fb, fbb);
    
    FBfreebltpblk(fbb);
  } 
  if(partp->data.graphics.data == NULL || partp->data.graphics.border) 
#endif
{
    if(partp->data.graphics.border_colour & 0x80000000)
      colour = info->default_text_colour;
    else
      colour = partp->data.graphics.border_colour;

    fprintf(info->outfile,"newpath\n");

    psdump_set_color(colour);

    fprintf(info->outfile,
	    "%d %d moveto\n"
	    "%d %d lineto\n"
	    "%d %d lineto\n"
	    "%d %d lineto\n"
	    "%d %d lineto\n"
	    "stroke\n",
	    x1, y1,  /* move to upper-left corner */
	    x1, y2,  /* line to lower-left corner */
	    x2, y2,  /* line to lower-right corner */
	    x2, y1,  /* line to upper-right corner */
	    x1, y1); /* line to upper-left corner */

    /* These lineto's could have been relative:
     * width height rlineto
     * but that is not the way psdump_translate_[xy] work
     * right now.
     */

  }

  return 0;
}
