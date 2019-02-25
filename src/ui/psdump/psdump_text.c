/**
 * Function to render text on the Postscript dump user interface.
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

#include <string.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Render a piece of text on the display.
 *
 * @param partp A pointer to the layout part representing the text.
 * @param geometry The current geometry of the display, describing what
 * @param geometry and where to render the text.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_draw_text(struct layout_part *partp /* , */
/* 		     struct layout_rectangle geometry, */
/* 		     struct layout_rectangle scroll */)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
  int i, x_position, y_position;
  uint32_t colour;
  char font[30];

  /* An empty string is pointless to draw. */
  if(partp->data.text.text[0] == '\0')
    return 0;

  /* Check if we are to use the default colour for this text. */
  if(partp->data.text.style.colour & 0x80000000)
    colour = info->default_text_colour;
  else if(partp->data.text.style.colour & 0x40000000)
    colour = info->default_link_colour;
  else
    colour = partp->data.text.style.colour;

  psdump_choose_font(partp, font);

  /* Only set font if another was used last time.
   */
  if (!strcmp(font, info->lastfont) &&
      partp->data.text.style.size == info->lastfontsize) {
    /* Same font, do nothing. */
  } else {
    fprintf(info->outfile,
	    "/%s findfont\n"
	    "%d scalefont\n"
	    "setfont\n", font, partp->data.text.style.size);
    strcpy(info->lastfont, font);
    info->lastfontsize = partp->data.text.style.size;
  }

  psdump_set_color(colour);

  /* The Y position is the baseline of the characters. Therefore,
   * add font size to Y coordinate.
   */
  x_position = psdump_translate_x(partp->geometry.x_position);
  y_position = psdump_translate_y(partp->geometry.y_position + partp->data.text.style.size);

  fprintf(info->outfile,
	  "%d %d moveto\n"
	  "(", x_position, y_position);

  /* Parantheses and backslashes are to be escaped with a
   * backslash.
   */
  for (i = 0; i < strlen(partp->data.text.text); i++) {
    switch (partp->data.text.text[i]) {
    case '(':
    case ')':
    case '\\':
      fprintf(info->outfile,"\\%c", partp->data.text.text[i]);
      break;
    default:
      fprintf(info->outfile,"%c", partp->data.text.text[i]);
      break;
    }
  }

  fprintf(info->outfile,") show\n");

  /* Draw a line under the text, if it is specified to be underlined. */
  if(partp->data.text.style.underlined) {
    fprintf(info->outfile,
	    "0.5 setlinewidth\n"
	    "%d %d %d %d drawline\n",
	    psdump_translate_x(partp->geometry.x_position),
	    psdump_translate_y(partp->geometry.y_position + partp->data.text.style.size + 1),
	    psdump_translate_x(partp->geometry.x_position + partp->geometry.width),
	    psdump_translate_y(partp->geometry.y_position + partp->data.text.style.size + 1));

  }

  return 0;
}

  
