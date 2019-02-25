/**
 * Function to print the title of the page on the oFBis user interface.
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
#include <ofbis.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Display the title in the upper right corner of the screen.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_set_title(char *title_text)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;
  int i, x_position;
  uint32_t text_colour, bg_colour;
  FBFONT font;

  if(title_text == NULL)
    return 1;

  fb = info->ofbis_fb;

  font.width = fontwidth_8x16;
  font.height = fontheight_8x16;
  font.data = fontdata_8x16;
  FBsetfont(fb, &font);

  text_colour = FBc24_to_cnative(fb, info->control_field_text_colour);
  bg_colour = FBc24_to_cnative(fb, info->control_field_colour);

  x_position = ofbis_ui->ui_display.width - 8 * strlen(title_text);
  for(i = 0 ; i < strlen(title_text) ; i++) {
    FBputchar(fb, x_position, 0, text_colour, bg_colour, title_text[i]);
    x_position += 8;
  }

  return 0;
}

/**
 * Display the URL of the page in the upper left corner of the screen.
 *
 * @param url The URL of the page.
 * 
 * @return non-zero value if an error occurred.
 */
int ofbis_set_url(char *url)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;
  int i, x_position;
  uint32_t text_colour, bg_colour;
  FBFONT font;

  if(url == NULL)
    return 1;

  fb = info->ofbis_fb;

  font.width = fontwidth_8x16;
  font.height = fontheight_8x16;
  font.data = fontdata_8x16;
  FBsetfont(fb, &font);

  text_colour = FBc24_to_cnative(fb, info->control_field_text_colour);
  bg_colour = FBc24_to_cnative(fb, info->control_field_colour);

  x_position = 0;
  for(i = 0 ; i < strlen(url) ; i++) {
    FBputchar(fb, x_position, 0, text_colour, bg_colour, url[i]);
    x_position += 8;
  }

  return 0;
}

/**
 * Display a text in the status area of the control field.
 *
 * @param text The status text to display.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_set_status_text(char *text)
{
#ifdef USE_CONTROL_FIELD
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;
  int i, x_position;
  uint32_t text_colour, bg_colour;
  FBFONT font;
  struct layout_rectangle control_geometry;

  /* Clear the part of the display to write on in the control field. */
  control_geometry.x_position = 0;
  control_geometry.y_position = 16;
  control_geometry.width = ofbis_ui->ui_display.width;
  control_geometry.height = 16;
  ofbis_fill_area(control_geometry, info->control_field_colour);

  if(text == NULL || strlen(text) == 0)
    return 1;

  fb = info->ofbis_fb;

  font.width = fontwidth_8x16;
  font.height = fontheight_8x16;
  font.data = fontdata_8x16;
  FBsetfont(fb, &font);

  text_colour = FBc24_to_cnative(fb, info->control_field_text_colour);
  bg_colour = FBc24_to_cnative(fb, info->control_field_colour);

  x_position = 0;
  for(i = 0 ; i < strlen(text) ; i++) {
    FBputchar(fb, x_position, 16, text_colour, bg_colour, text[i]);
    x_position += 8;
  }
#endif

  return 0;
}

