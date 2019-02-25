/**
 * Close function for the GTK+ user interface.
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

#include <stdlib.h>
#include <gtk/gtk.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Safely close down the GTK+ user interface and its windows.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_close(void)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  int i;

  if(info->progress_image_amount > 0) {
    if(info->progress_images != NULL) {
      for(i = 0 ; i <= 93 ; i++)
	gdk_pixmap_unref(info->progress_images[i]);
      free(info->progress_images);
    }
    
    if(info->progress_masks != NULL) {
      for(i = 0 ; i <= 93 ; i++)
	gdk_pixmap_unref(info->progress_masks[i]);
      free(info->progress_masks);
    }
  }

  return 0;
}

/**
 * Function used in some cases to safely free all reserved interface
 * specific data, which has been placed in layout parts, throughout
 * the GTK+ interface.
 *
 * @param part A pointer to the layout part to free data in. 
 */
void gtkui_free_interface_data(struct layout_part *partp)
{
  switch(partp->type) {
  case LAYOUT_PART_PAGE_INFORMATION:
    break;

  case LAYOUT_PART_TEXT:
    gdk_font_unref(((struct gtkui_text_information *)
		    partp->interface_data)->font);
    gdk_gc_unref(((struct gtkui_text_information *)
		  partp->interface_data)->text_gc);
    gdk_gc_unref(((struct gtkui_text_information *)
		  partp->interface_data)->background_gc);
    break;

  case LAYOUT_PART_LINE:
    gdk_gc_unref(((struct gtkui_line_information *)
		  partp->interface_data)->light_gc);
    gdk_gc_unref(((struct gtkui_line_information *)
		  partp->interface_data)->dark_gc);
    gdk_gc_unref(((struct gtkui_line_information *)
		  partp->interface_data)->background_gc);
    break;

  case LAYOUT_PART_GRAPHICS:
    gdk_gc_unref(((struct gtkui_image_information *)
		  partp->interface_data)->image_gc);
    gdk_gc_unref(((struct gtkui_image_information *)
		  partp->interface_data)->border_gc);
    gdk_gc_unref(((struct gtkui_image_information *)
		  partp->interface_data)->background_gc);
    break;

  default:
    break;
  }

  if(partp->interface_data) {
    free(partp->interface_data);
    partp->interface_data = NULL;
  }
}
