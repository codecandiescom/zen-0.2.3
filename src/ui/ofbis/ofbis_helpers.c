/**
 * Functions to help the oFBis user interface on its course into perfection.
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
#include <string.h>
#include <sys/types.h>
#include <ofbis.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * Sets one colour in the palette of the display, if it is
 * palette based.
 *
 * @param index The index of the colour to set.
 * @param colour The 24-bit value of new colour.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_set_colour(int index, uint32_t colour)
{
  FB *fb = ((struct ofbis_information *)ofbis_ui->ui_specific)->ofbis_fb;
  FBCMAP *fbcmap;

  if(ofbis_ui->ui_display.colourmap == NULL)
    return 1;

  if(index > (1 << ofbis_ui->ui_display.bit_depth))
    return 1;

  fbcmap = FBgetcmap(fb);
  if(fbcmap == NULL)
    return 1;

  fbcmap->red[index] =   
    (((colour >> 16) & 0xff) << 8) | ((colour >> 16) & 0xff);
  fbcmap->green[index] =
    (((colour >>  8) & 0xff) << 8) | ((colour >>  8) & 0xff);
  fbcmap->blue[index] = 
    (((colour >>  0) & 0xff) << 8) | ((colour >>  0) & 0xff);

  /* Update the colourmap in the user interface too. */
  ofbis_ui->ui_display.colourmap[3 * index + 0] = (colour >> 16) & 0xff;
  ofbis_ui->ui_display.colourmap[3 * index + 1] = (colour >>  8) & 0xff;
  ofbis_ui->ui_display.colourmap[3 * index + 2] = (colour >>  0) & 0xff;

  fbcmap->start = index;
  fbcmap->end = index;
  fbcmap->len = 1;

  FBputcmap(fb, fbcmap);
  FBfreecmap(fbcmap);

  return 0;  
}

/**
 * Set the palette of the display, if it is palette based.
 *
 * @param red An array of colours to fill the red component.
 * @param green An array of colours to fill the green component.
 * @param blue An array of colours to fill the blue component.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_set_palette(int *red, int *green, int *blue)
{
  FB *fb = ((struct ofbis_information *)ofbis_ui->ui_specific)->ofbis_fb;
  FBCMAP *fbcmap;
  int i, nr_of_colours;

  if(ofbis_ui->ui_display.bit_depth > 8)
    return 1;

  nr_of_colours = 1 << ofbis_ui->ui_display.bit_depth;

  fbcmap = FBgetcmap(fb);
  if(fbcmap == NULL)
    return 1;

  for(i = 0 ; i < nr_of_colours ; i++) {
    fbcmap->red[i] = red[i] << 8;
    fbcmap->green[i] = green[i] << 8;
    fbcmap->blue[i] = blue[i] << 8;
  }

  fbcmap->start = 0;
  fbcmap->end = nr_of_colours - 1;
  fbcmap->len = nr_of_colours;

  FBputcmap(fb, fbcmap);
  FBfreecmap(fbcmap);

  return 0;
}

/**
 * Set the width and height of a part, which only the user interface
 * can know the width and height of. This includes text strings and
 * various widgets in forms.
 *
 * @param partp A pointer to the layout part to set the size of.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_set_size(struct layout_part *partp)
{
  switch(partp->type) {
  case LAYOUT_PART_TEXT:
    /* All current fonts in oFBis are always 8 pixels wide. */
    partp->geometry.width = 8 * strlen(partp->data.text.text);
    partp->geometry.height = partp->data.text.style.size;
    break;

  case LAYOUT_PART_FORM_SUBMIT:
    /* Just use the default fontsize here. It might be better to look
     * at the nearest text part, but then we'd have to search for it in
     * some good way. This will do for now. 
     */
    partp->geometry.width = 8 * strlen(partp->data.form_submit.value) + 2 * 7;
    partp->geometry.height = ofbis_ui->ui_settings.default_fontsize + 2 * 3;
    break;

  default:
    break;
  }

  return 0;
}

/**
 * Fills an area of the screen with a specific colour.
 *
 * @param geometry A rectangle specifying the area to fill.
 * @param colour The 24-bit value of colour to fill with.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_fill_area(struct layout_rectangle geometry, uint32_t fill_colour)
{
  FB *fb = ((struct ofbis_information *)ofbis_ui->ui_specific)->ofbis_fb;
  uint32_t colour;
  unsigned char *buffer_8;
  uint16_t *buffer_16;
  uint32_t *buffer_32;
  int i;
  FBBLTPBLK *fbb;

  colour = FBc24_to_cnative(fb, fill_colour);

  /* Should be enough for one row. */
  buffer_8 = (unsigned char *)malloc(16384);
  if(buffer_8 == NULL)
    return 1;
  buffer_16 = (uint16_t *)buffer_8;
  buffer_32 = (uint32_t *)buffer_8;

  switch(ofbis_ui->ui_display.bit_depth) {
  case 8:
    for(i = 0 ; i < 16384 ; i++) {
      buffer_8[i] = (unsigned char)colour;
    }
    break;
    
  case 16:
    for(i = 0 ; i < 16384 / 2 ; i++) {
      buffer_16[i] = (uint16_t)colour;
    }
    break;

  case 24:
    for(i = 0 ; i < 16384 ; i += 3) {
      buffer_8[i + 0] = (unsigned char)((colour >> 16) & 0xff);
      buffer_8[i + 1] = (unsigned char)((colour >>  8) & 0xff);
      buffer_8[i + 2] = (unsigned char)((colour >>  0) & 0xff);
    }
    break;

  case 32:
    for(i = 0 ; i < 16384 / 4 ; i++) {
      buffer_32[i] = colour;
    }
    break;

  default:
    fprintf(stderr, "Unsupported bit depth for filling.\n");
  }

  fbb = FBgetbltpblk(fb);
  fbb->b_wd = min(geometry.width, ofbis_ui->ui_display.width);
  fbb->b_ht = min(geometry.height, ofbis_ui->ui_display.height);
  fbb->plane_ct = ofbis_ui->ui_display.bit_depth;
  fbb->d_xmin = geometry.x_position;
  fbb->d_ymin = geometry.y_position;
  fbb->s_form = buffer_16;
  fbb->s_nxwd = ofbis_ui->ui_display.bit_depth * 2;
  fbb->s_nxln = 0;
  fbb->s_nxpl = 2;    
  FBbitblt(fb, fbb);
  FBfreebltpblk(fbb);

  free(buffer_8);

  return 0;  
}

/**
 * Clip a part's rectangle geometry against the display geometry.
 *
 * @param part The geometry of the part.
 * @param geometry The geometry of the display.
 * @param scroll The current scroll offset.
 * @param cut A pointer to a geometry rectangle which will contain the
 * @param cut clipped rectangle, always relative to the original part.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_cut_rectangle(struct layout_rectangle part, 
			struct layout_rectangle geometry,
			struct layout_rectangle scroll,
			struct layout_rectangle *cut)
{
  int start_x, start_y, end_x, end_y;

  part.x_position -= scroll.x_position;
  part.y_position -= scroll.y_position;

  start_x = part.x_position - geometry.x_position;
  start_y = part.y_position - geometry.y_position;
  cut->x_position = max(0, -start_x);
  cut->y_position = max(0, -start_y);

  start_x = max(part.x_position, geometry.x_position);
  start_y = max(part.y_position, geometry.y_position);

  end_x = min(geometry.x_position + geometry.width - 1, 
	      part.x_position + part.width - 1);
  end_y = min(geometry.y_position + geometry.height - 1, 
	      part.y_position + part.height - 1);

  cut->width = end_x - start_x + 1;
  cut->height = end_y - start_y + 1;

  return 0;
}
