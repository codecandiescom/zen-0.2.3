/**
 * Functions to handle fonts in the GTK+ user interface.
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
#include <string.h>
#include <gtk/gtk.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Provide a GDK font, based on an X font name. This might later 
 * change to take a font face name, used in HTML code. There might
 * also be a more central convertion routine for this. Only time
 * will tell. A value of NULL here, will give an Helvetica font.
 * Also, if the given font name is not found, Helvetica, or the 
 * first, best font is used.
 *
 * @param textp A pointer to a layout part struct, containing the
 * @param textp information we need to find the right font.
 *
 * @return a pointer to a GdkFont, or NULL, if no font was found.
 */
GdkFont *gtkui_get_font(struct layout_part *textp)
{
  GdkFont *font;
  char font_name[256], weight[16], slant[4], family[64];
  int font_size;

  font_size = textp->data.text.style.size;

  if(textp->data.text.style.bold)
    strcpy(weight, "bold");
  else
    strcpy(weight, "medium");

  if(textp->data.text.style.italic)
    strcpy(slant, "i");
  else
    strcpy(slant, "r");

  if(textp->data.text.style.monospaced) {
    strcpy(family, "courier");
    font_size += 2;
  } else {
    strcpy(family, "helvetica");
  }    

  sprintf(font_name, 
	  "-*-%s-%s-%s-normal--%d-*-*-*-*-*-iso8859-1",
	  family, weight, slant, font_size);
  font = gdk_font_load(font_name);

  /* Nothing appropriate found. To to find another one, by removing
   * the slant requirement. 
   */
  if(font == NULL) {
    sprintf(font_name, 
	    "-*-%s-%s-*-normal--%d-*-*-*-*-*-iso8859-1",
	    family, weight, font_size);
    font = gdk_font_load(font_name);
  }
   
  /* Still nothing found. Let us remove the weight feature too. */
  if(font == NULL) {
    sprintf(font_name, 
	    "-*-%s-*-*-normal--%d-*-*-*-*-*-iso8859-1",
	    family, font_size);
    font = gdk_font_load(font_name);
  }

  /* Does the user have no fonts at all installed? We now skip
   * the font family, but still keep it to a normally condensed
   * ISO 8859-1 font.
   */
  if(font == NULL) {
    sprintf(font_name, 
	    "-*-*-*-*-normal--%d-*-*-*-*-*-iso8859-1",
	    font_size);
    font = gdk_font_load(font_name);
  }
   
  /* And finally, I give up, give me the first one you have. */
  if(font == NULL) {
    font = gdk_font_load("-*-*-*-*-*--*-*-*-*-*-*-*-*");
  }

  if(font == NULL) {
    fprintf(stderr, "Error! Correct font was not found! "
	    "size=%d, bold=%d, italic=%d, monospaced=%d\n",
	    textp->data.text.style.size,
	    textp->data.text.style.bold,
	    textp->data.text.style.italic,
	    textp->data.text.style.monospaced);
  }

  return font;
}
