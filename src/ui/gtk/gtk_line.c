/**
 * Function to draw a horizontal line on the GTK+ user interface.
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
#include <gtk/gtk.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * This is the function that will do the actual drawing of the line. 
 * It is called from GTK+ when the line, or part of the line have to
 * be redrawn, and when we are being asked so nicely for such a thing,
 * we can do nothing else but to obey.
 *
 * @param widget The widget which needs redrawing.
 * @param event The event data for the redrawing.
 *
 * @return
 */
static gint render_line_event(GtkWidget *widget, GdkEventExpose *event,
			      gpointer data)
{
  GtkLayout *display;
  struct layout_part *partp;
  struct layout_rectangle rect;
  struct gtkui_line_information *line_information;
  GdkGC *light_gc, *dark_gc;

  display = GTK_LAYOUT(widget);
  partp = (struct layout_part *)data;
  line_information = (struct gtkui_line_information *)partp->interface_data;
  light_gc = line_information->light_gc;
  dark_gc = line_information->dark_gc;

  gdk_gc_set_clip_rectangle(light_gc, &event->area);
  gdk_gc_set_clip_rectangle(dark_gc, &event->area);

  rect.x_position = partp->geometry.x_position + 8 - display->xoffset;
  rect.y_position = partp->geometry.y_position - display->yoffset + 
    (partp->geometry.height - partp->data.line.size) / 2;
  rect.width = partp->geometry.width - 16;
  rect.height = partp->data.line.size;

#ifdef DEBUG
  fprintf(stderr, "%s(%d): line %dx%d+%d+%d\n", __FUNCTION__, __LINE__, 
	  rect.width, rect.height, rect.x_position, rect.y_position);
#endif

  gdk_draw_line(display->bin_window, light_gc,
		rect.x_position + 1,
		rect.y_position + rect.height - 1,
		rect.x_position + rect.width - 2,
		rect.y_position + rect.height - 1);
  gdk_draw_line(display->bin_window, light_gc,
		rect.x_position + rect.width - 1,
		rect.y_position,
		rect.x_position + rect.width - 1,
		rect.y_position + rect.height - 1);

  gdk_draw_line(display->bin_window, dark_gc,
		rect.x_position,
		rect.y_position,
		rect.x_position + rect.width - 2,
		rect.y_position);
  gdk_draw_line(display->bin_window, dark_gc,
		rect.x_position,
		rect.y_position,
		rect.x_position,
		rect.y_position + rect.height - 1);

  return FALSE;
}

/**
 * Draws a simple horizontal line as wide as specified.
 *
 * @param partp A pointer to the layout part representing the line.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_line(struct layout_part *partp, GtkLayout *display)
{
  struct gtkui_line_information *line_information;
  GdkGC *light_gc, *dark_gc, *bg_gc;
  GdkColor light_colour, dark_colour, background_colour;
  uint32_t *bg_colourp;

  line_information = (struct gtkui_line_information *)
    malloc(sizeof(struct gtkui_line_information));
  if(line_information == NULL)
    return 1;

  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), 0xeeeeee, &light_colour);
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), 0x666666, &dark_colour);

  light_gc = gdk_gc_new(display->bin_window);
  dark_gc = gdk_gc_new(display->bin_window);
  bg_gc = gdk_gc_new(display->bin_window);
  gdk_gc_set_foreground(light_gc, &light_colour);
  gdk_gc_set_foreground(dark_gc, &dark_colour);

  /* Attract the current background colour value from the current display. */
  bg_colourp = gtk_object_get_data(GTK_OBJECT(display), "background_colour");
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), *bg_colourp, 
				&background_colour);
  gdk_gc_set_background(light_gc, &background_colour);
  gdk_gc_set_background(dark_gc, &background_colour);
  gdk_gc_set_foreground(bg_gc, &background_colour);

  line_information->light_gc = light_gc;
  line_information->dark_gc = dark_gc;
  line_information->background_gc = bg_gc;
  partp->interface_data = (void *)line_information;
  partp->free_interface_data = gtkui_free_interface_data;

  gtk_signal_connect(GTK_OBJECT(display), "expose_event",
		     (GtkSignalFunc)render_line_event, (gpointer)partp);

  return 0;
}

