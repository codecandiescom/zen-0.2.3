/**
 * Function to print text for the dump user interface.
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

#include "layout.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Print out a piece of plain text.
 *
 * @return non-zero value if an error occurred.
 */
int dump_draw_text(struct layout_part *partp)
{
  static int previous_y_position = 0;
  static int previous_x_position = 0;
  int i;

  if(partp->geometry.y_position > previous_y_position) {
    for(i = 0 ; 
	i < (partp->geometry.y_position - previous_y_position) / 8 ; 
	i++)
      printf("\n");

    previous_y_position = partp->geometry.y_position;
    previous_x_position = 0;
  }

  if(partp->data.text.text) {
    for(i = 0 ; 
	i < (partp->geometry.x_position - previous_x_position) / 8 ; 
	i++) {
      printf(" ");
    }
    printf("%s", partp->data.text.text);
    previous_x_position += i * 8 + partp->geometry.width;
  }

  return 0;
}
