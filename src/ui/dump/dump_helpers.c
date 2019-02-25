/**
 * Functions to help the plain text dump user interface with various 
 * things that the other parts of it cannot handle, for some reason.
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

#include "layout.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Set the width and height of a part, which only the user interface
 * can know the width and height of. This includes text strings and
 * various widgets in forms.
 *
 * @param partp A pointer to the layout part to set the size of.
 *
 * @return non-zero value if an error occurred.
 */
int dump_set_size(struct layout_part *partp)
{
  switch(partp->type) {
  case LAYOUT_PART_TEXT:
    /* We define the character size to be 8x8 pixels. */
    partp->geometry.width = 8 * strlen(partp->data.text.text);
    partp->geometry.height = 8;
    break;

  default:
    break;
  }

  return 0;
}
