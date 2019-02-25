/**
 * Function to draw an image on the GTK+ user interface.
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
#include <string.h>
#include <gtk/gtk.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * This is the function that will do the actual drawing of the image. 
 * It is called from GTK+ when the image, or part of the image have to
 * be redrawn. We obey, of course, to the God of GTK+, and draw.
 *
 * @param widget The widget which needs redrawing.
 * @param event The event data for the redrawing.
 * @param user_data Arbitrary data sent to the event function. Unused.
 *
 * @return
 */
static gboolean render_image_event(GtkWidget *widget, GdkEventExpose *event,
				   gpointer data)
{
  struct layout_part *partp;
  struct gtkui_image_information *image_information;
  GtkLayout *display;
  GdkGC *gc, *border_gc;
  GdkWindow *window;
  GdkRectangle partrect, clipping;

  if(GTK_IS_DRAWING_AREA(widget)) {
    partp = (struct layout_part *)
      gtk_object_get_data(GTK_OBJECT(widget), "layout_part");
    display = GTK_LAYOUT(data);

    partrect.x = 0;
    partrect.y = 0;
    partrect.width = partp->geometry.width;
    partrect.height = partp->geometry.height;
    clipping = partrect;

    window = widget->window;
  } else {
    partp = (struct layout_part *)data;
    display = GTK_LAYOUT(widget);    

    partrect.x = partp->geometry.x_position - display->xoffset;
    partrect.y = partp->geometry.y_position - display->yoffset;
    partrect.width = partp->geometry.width;
    partrect.height = partp->geometry.height;
    if(!gdk_rectangle_intersect(&event->area, &partrect, &clipping))
      return FALSE;

    window = display->bin_window;
  }

  image_information = (struct gtkui_image_information *)partp->interface_data;
  gc = image_information->image_gc;
  border_gc = image_information->border_gc;

  gdk_gc_set_clip_rectangle(gc, &clipping);
  gdk_gc_set_clip_rectangle(border_gc, &clipping);

  /* Do not draw an image, if there is no image to draw. */
  if(partp->data.graphics.data != NULL) {
    gdk_draw_rgb_image(window, gc,
		       partrect.x, partrect.y, 
		       partrect.width, partrect.height,
		       GDK_RGB_DITHER_NORMAL, partp->data.graphics.data,
		       3 * partrect.width);
  }

  if(partp->data.graphics.border > 0) {
    gdk_draw_rectangle(window, border_gc, FALSE, 
		       partrect.x, partrect.y,
		       partrect.width - 1, partrect.height - 1);
  }  

  return FALSE;
}

/**
 * Changes the colour of the graphics context associated with the
 * text. Then redraw the text in the new colour. 
 *
 * @param widget
 * @param event
 * @param user_data
 *
 * @return
 */
static gboolean activate_image_link(GtkWidget *widget, 
				    GdkEventCrossing *event, 
				    gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  GtkLayout *display;
  GtkWidget *drawing_area;
  struct layout_part *partp;
  struct gtkui_image_information *image_information;
  GdkGC *border_gc;
  uint32_t colour_value;
  GdkColor colour;
  GdkEventExpose expose_event;

  drawing_area = GTK_WIDGET(data);
  partp = (struct layout_part *)gtk_object_get_data(GTK_OBJECT(drawing_area), 
						    "layout_part");
  display = info->display;

  image_information = (struct gtkui_image_information *)partp->interface_data;
  border_gc = image_information->border_gc;

  if(event->type == GDK_ENTER_NOTIFY) {
    colour_value = info->default_active_link_colour;
    gtkui_set_status_text(partp->parent->data.link.href);
  } else {
    colour_value = partp->data.graphics.border_colour;
    gtkui_set_status_text(NULL);
  }

  gtkui_colourvalue_to_gdkcolor(widget, colour_value, &colour);
  gdk_gc_set_foreground(border_gc, &colour);

  expose_event.type = GDK_EXPOSE;
  expose_event.send_event = TRUE;
  expose_event.window = drawing_area->window;
  expose_event.area.x = drawing_area->allocation.x;
  expose_event.area.y = drawing_area->allocation.y;
  expose_event.area.width = drawing_area->allocation.width;
  expose_event.area.height = drawing_area->allocation.height;
  render_image_event(drawing_area, &expose_event, (gpointer)display);

  return TRUE;
}

/**
 *
 */
static gboolean select_image_link(GtkWidget *widget, GdkEventButton *event,
				  gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  struct layout_part *partp;
  static int button_pressed = 0;
  unsigned char *status_text;

  partp = (struct layout_part *)data;

  if(event->type == GDK_BUTTON_PRESS) {
    button_pressed = 1;
  } else if(button_pressed) {
    button_pressed = 0;
    
    /* Request the URL that the user clicked upon. */
    status_text = (unsigned char *)
      malloc(strlen(partp->parent->data.link.href) + 16);
    if(status_text == NULL)
      return FALSE;

    info->page_id = gtkui_ui->ui_functions->
      request_page(partp->parent->data.link.href,
		   info->current_page->data.page_information.url);
    sprintf(status_text, "Loading: %s", partp->parent->data.link.href);
    gtkui_set_status_text(status_text);

    free(status_text);
  }

  return TRUE;
}

/**
 * Draw an image on the display. If the image could not be loaded for some 
 * reason, we only draw an empty box.
 * GTK+ can only deal with XPMs which means we have to convert the image
 * we get from the main program, into a datastructure conforming to XPM.
 * Fortunately, XPM is an easy format. If I write XPM one more time, it
 * would be the fourth one in this block of comments. 
 *
 * @param partp A pointer to the layout part representing the image.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_image(struct layout_part *partp,
		     GtkLayout *display)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;  
  struct gtkui_image_information *image_information;
  GtkWidget *drawing_area;
  GdkWindow *window;
  GdkGC *gc, *border_gc, *bg_gc;
  GdkColor colour;
  uint32_t *bg_colourp;
  int is_link;

  if(partp->parent && partp->parent->type == LAYOUT_PART_LINK)
    is_link = 1;
  else
    is_link = 0;

  /* If the image is a link, we connect the link activation event to
   * this image, by putting it inside its own event box and drawing area. 
   */
  if(is_link) {
    drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area),
			  partp->geometry.width, partp->geometry.height);
    gtk_object_set_data(GTK_OBJECT(drawing_area), "layout_part", partp);
    gtk_layout_put(display, drawing_area, 
		   partp->geometry.x_position, partp->geometry.y_position);

    gtk_signal_connect_object(GTK_OBJECT(display), "expose_event",
			      (GtkSignalFunc)render_image_event, 
			      GTK_OBJECT(drawing_area));
    gtk_signal_connect(GTK_OBJECT(drawing_area), "enter_notify_event",
		       (GtkSignalFunc)activate_image_link, drawing_area);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "leave_notify_event",
		       (GtkSignalFunc)activate_image_link, drawing_area);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "button_press_event",
		       (GtkSignalFunc)select_image_link, partp);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "button_release_event",
		       (GtkSignalFunc)select_image_link, partp);
    gtk_widget_add_events(drawing_area, 
			  GDK_ENTER_NOTIFY_MASK | 
			  GDK_LEAVE_NOTIFY_MASK |
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK);
    gtk_widget_realize(drawing_area);
    gdk_window_set_cursor(drawing_area->window, gdk_cursor_new(GDK_HAND2));
    gtk_widget_show(drawing_area);
    
    window = drawing_area->window;
  } else {
    gtk_signal_connect(GTK_OBJECT(display), "expose_event",
		       (GtkSignalFunc)render_image_event, (gpointer)partp);
    gtk_widget_add_events(GTK_WIDGET(display), GDK_EXPOSURE_MASK);
    window = display->bin_window;

    drawing_area = NULL;
  }

  image_information = (struct gtkui_image_information *)
    malloc(sizeof(struct gtkui_image_information));
  if(image_information == NULL) {
    if(drawing_area)
      gtk_widget_destroy(drawing_area);
    return 1;
  }

  gc = gdk_gc_new(window);
  border_gc = gdk_gc_new(window);
  bg_gc = gdk_gc_new(window);

  if(partp->data.graphics.border_colour & 0x80000000)
    partp->data.graphics.border_colour = info->default_text_colour;
  else if(partp->data.graphics.border_colour & 0x40000000)
    partp->data.graphics.border_colour = info->default_link_colour;

  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display),
				partp->data.graphics.border_colour, &colour);
  gdk_gc_set_foreground(border_gc, &colour);

  /* Attract the current background colour value from the current display. */
  bg_colourp = gtk_object_get_data(GTK_OBJECT(display), "background_colour");
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), *bg_colourp, &colour);
  gdk_gc_set_foreground(bg_gc, &colour);

  image_information->image_gc = gc;
  image_information->border_gc = border_gc;
  image_information->background_gc = bg_gc;
  partp->interface_data = (void *)image_information;
  partp->free_interface_data = gtkui_free_interface_data;

  return 0;
}
