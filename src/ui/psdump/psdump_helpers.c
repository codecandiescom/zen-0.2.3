/**
 * Functions to help the Postscript dump user interface on its
 * course into perfection.
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * Translates Zen X coordinates into Postscript points, according 
 * to defined margins and stuff.
 *
 * Comment: Should this return floating point values, for greater
 *          precision?
 *
 * @param x Zen X coordinate to be translated
 * @return Postscript X coordinate
 */
int psdump_translate_x(int x)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
#ifdef DEBUG
  fprintf(stderr, "%s: translating Zen X %d to Postscript X %d\n",
	  __FUNCTION__, x, x + info->left_margin);
#endif /* DEBUG */
  return x + info->left_margin;
}

/**
 * Translates Zen Y coordinates into Postscript points, according 
 * to defined margins and stuff. Also takes into account the fact
 * that in Postscript a Y coordinate of 0 means bottom of the page.
 *
 * Comment: Should this return floating point values, for greater
 *          precision?
 *
 * @param y Zen Y coordinate to be translated
 * @return Postscript Y coordinate
 */
int psdump_translate_y(int y)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
#ifdef DEBUG
  fprintf(stderr, "%s: translating Zen Y %d to Postscript Y %d\n",
	  __FUNCTION__, y, (info->bottom_margin + psdump_ui->ui_display.height) - (y
	  - info->y_distance_printed));
#endif /* DEBUG */
  return (info->bottom_margin + psdump_ui->ui_display.height) - (y - info->y_distance_printed);
}

/**
 * Translates a relative X distance from Postscript points to Zen
 * coordinates. Currently it does nothing, but it might do in
 * the future, if scaling of some kind is introduced.
 *
 * @param x X distance to be untranslated
 * @return distance in Zen coordinates
 */
int psdump_untranslate_width(double x) {
  return (int)x;
}

/**
 * Converts a uint32_t colour value to a Postscript setrgbcolor
 * command.
 *
 * @param colour Colour to be converted, as 0x00rrggbb
 */
void psdump_set_color(uint32_t colour)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;

  /* Has this colour been set before? If so, don't set it.
   */
  if (info->lastcolour == 0xff000000 ||
      info->lastcolour != colour) {
    fprintf(info->outfile,
	    "%f %f %f setrgbcolor\n",
	    (double)((colour & 0x00ff0000) >> 16) / 255.0,
	    (double)((colour & 0x0000ff00) >> 8) / 255.0,
	    (double)((colour & 0x000000ff) >> 0) / 255.0);
    info->lastcolour = colour;
  }
}

/**
 * Ejects the current page and starts a new one, by outputting
 * a "showpage" command and resetting various things.
 *
 * @param next_y Zen Y coordinate of the first layout part to be
 * @param next_y put on the new page
 */
void psdump_new_page(int next_y)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;

  fprintf(info->outfile, "grestore\n"
                         "showpage\n");
  info->page_number++;
  fprintf(info->outfile, "%%%%Page: %d %d\n"
                         "gsave\n", info->page_number, info->page_number);

  info->y_distance_printed = next_y;

  /* Pages should be independent. Therefore, wipe out information
   * about previously set fonts and colours.
   */
  info->lastfont[0] = '\0';
  info->lastfontsize = 0;
  info->lastcolour = 0xff000000;
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
int psdump_set_size(struct layout_part *partp)
{
  switch(partp->type) {
  case LAYOUT_PART_TEXT:
    {
      char font[30];
      double width;

      psdump_choose_font(partp, font);
      width = psdump_get_string_width(partp->data.text.text, font, partp->data.text.style.size);
      if (width < 0)
	partp->geometry.width = (partp->data.text.style.size/2) * strlen(partp->data.text.text);
      else
	partp->geometry.width = psdump_untranslate_width(width);
      partp->geometry.height = partp->data.text.style.size;

      break;
    }
  case LAYOUT_PART_FORM_SUBMIT:
    partp->geometry.width = 8 * strlen(partp->data.form_submit.value) + 2 * 7;
/*     partp->geometry.height = partp->data.text.style.size + 2 * 3; */
    partp->geometry.height = 12;
    break;

  default:
    break;
  }

  return 0;
}
