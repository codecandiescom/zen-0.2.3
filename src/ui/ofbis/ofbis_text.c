/**
 * Function to render text on the oFBis user interface.
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

#include <string.h>
#include <ofbis.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Render a piece of text on the display.
 * Supports underlined style. 
 *
 * Comments: Currently, this only supports one kind of monospaced font,
 *           which is the 8 by 8 pixel default font in oFBis. It will
 *           hopefully at least support True Type fonts in the future.
 *
 * @param partp A pointer to the layout part representing the text.
 * @param geometry The current geometry of the display, describing what
 * @param geometry and where to render the text.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_draw_text(struct layout_part *partp,
		    struct layout_rectangle geometry,
		    struct layout_rectangle scroll)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;
  int i, x_position, y_position;
  FBFONT font;
  uint32_t colour;

  fb = info->ofbis_fb;

  /* An empty string is pointless to draw. */
  if(partp->data.text.text[0] == '\0')
    return 0;

  /* Check if we are to use the default colour for this text. */
  if(partp->data.text.style.colour & 0x80000000)
    colour = FBc24_to_cnative(fb, info->default_text_colour);
  else if(partp->data.text.style.colour & 0x40000000)
    colour = FBc24_to_cnative(fb, info->default_link_colour);
  else
    colour = FBc24_to_cnative(fb, partp->data.text.style.colour);

  /* Select the best suited font size for this mission. */
  if(partp->data.text.style.size < 14) {
    font.width = fontwidth_8x8;
    font.height = fontheight_8x8;
    font.data = fontdata_8x8;
  } else if(partp->data.text.style.size < 16) {
    font.width = fontwidth_8x14;
    font.height = fontheight_8x14;
    font.data = fontdata_8x14;
  } else {
    font.width = fontwidth_8x16;
    font.height = fontheight_8x16;
    font.data = fontdata_8x16;
  }
  FBsetfont(fb, &font);

  x_position = partp->geometry.x_position - scroll.x_position + 
    info->x_position;
  y_position = partp->geometry.y_position - scroll.y_position +
    info->y_position;
  for(i = 0 ; i < strlen(partp->data.text.text) ; i++) {
    FBputchar(fb, x_position, y_position, 
	      colour, 0x80000000, 
	      partp->data.text.text[i]);

    /* Bold text is the same character, drawn with one pixel offset. */
    if(partp->data.text.style.bold) {
      FBputchar(fb, x_position + 1, y_position, 
		colour, 0x80000000, 
		partp->data.text.text[i]);
    }

    x_position += font.width;
  }

  /* Draw a line under the text, if it is specified to be underlined. */
  if(partp->data.text.style.underlined) {
    FBhline(fb, 
	    partp->geometry.x_position - scroll.x_position + info->x_position,
	    x_position - 1,
	    partp->geometry.y_position - scroll.y_position + font.height + 
	    info->y_position - 1,
	    colour);
  }

  return 0;
}

