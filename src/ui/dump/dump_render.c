/**
 * Contains functions to render a page with the dump user interface.
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

#include "zen_ui.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Render the page by calling the appropriate rendering functions.
 *
 * @param parts A pointer to the first layout part in the list to be rendered.
 *
 * @return non-zero value if an error occurred.
 */
int dump_render(struct layout_part *parts)
{
  struct layout_part *partp;

  partp = parts;

  while(partp) {

    /* Only the supported parts are represented here, all the others are
     * covered by the default statement, which does not do anything.
     */
    switch(partp->type) {
    case LAYOUT_PART_PAGE_INFORMATION:
      dump_set_title(partp->data.page_information.title);
      break;

    case LAYOUT_PART_TEXT:
      dump_draw_text(partp);
      break;

    case LAYOUT_PART_LINK:
      printf("[%s]", partp->data.link.href);
      break;

    case LAYOUT_PART_GRAPHICS:
      dump_draw_image(partp);
      break;

    case LAYOUT_PART_LINE:
      dump_draw_line(partp);
      break;

    default:
      break;
    }

    dump_render(partp->child);
    if(partp->type == LAYOUT_PART_PAGE_INFORMATION)
      break;

    partp = partp->next;
  }

  return 0;
}
