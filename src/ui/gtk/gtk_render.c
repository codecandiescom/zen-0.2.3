/**
 * Functions to render parts of a page in the GTK+ user interface.
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

#include "zen_ui.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * This function performs the actual drawing of a filled area.
 *
 * @param widget
 * @param event
 * @param user_data
 */
static void render_fill_area_event(GtkWidget *widget, GdkEventExpose *event,
				   gpointer data)
{
  struct gtkui_fill_information *fill_information;
  struct layout_rectangle rect;
  GtkLayout *display;

  display = GTK_LAYOUT(widget);

  fill_information = (struct gtkui_fill_information *)data;
  rect = fill_information->area;

  gdk_gc_set_clip_rectangle(fill_information->gc, &event->area);

#ifdef DEBUG
  fprintf(stderr, "%s(%d): fill area %dx%d+%d+%d\n", __FUNCTION__, __LINE__, 
	  rect.width, rect.height, rect.x_position, rect.y_position);
#endif

  gdk_draw_rectangle(display->bin_window, 
		     fill_information->gc, TRUE,
		     rect.x_position - display->xoffset, 
		     rect.y_position - display->yoffset, 
		     rect.width, rect.height);
}

/**
 * This sets up a drawing area covering a specified area, which will be
 * drawn by the render_fill_area_event() whenever it is needed.
 *
 * @param area The area which should be filled. 
 * @param display A pointer to the GtkLayout widget to draw on.
 * @param colour_value A 32-bit colour value with the colour to fill with.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_fill_area(struct layout_rectangle area, GtkLayout *display,
		    uint32_t colour_value)
{
  GdkGC *gc;
  GdkColor colour;
  struct gtkui_fill_information *fill_information;

  /* !!! Memory leak !!! */

  fill_information = (struct gtkui_fill_information *)
    malloc(sizeof(struct gtkui_fill_information));
  if(fill_information == NULL)
    return 1;

  gc = gdk_gc_new(display->bin_window);
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), colour_value, &colour);
  gdk_gc_set_foreground(gc, &colour);

  fill_information->gc = gc;
  fill_information->area = area;

  gtk_signal_connect(GTK_OBJECT(display), "expose_event",
		     (GtkSignalFunc)render_fill_area_event, fill_information);

  return 0;
}

/**
 * This function performs the actual drawing of a filled area.
 *
 * @param widget
 * @param event
 * @param user_data
 */
static void render_rectangle_event(GtkWidget *widget, GdkEventExpose *event,
				   gpointer data)
{
  GtkLayout *display;
  struct layout_rectangle rect;
  struct gtkui_rectangle_information *rectangle_information;

  display = GTK_LAYOUT(widget);
  rectangle_information = (struct gtkui_rectangle_information *)data;
  rect = rectangle_information->rectangle;
  rect.x_position -= display->xoffset;
  rect.y_position -= display->yoffset;

  gdk_gc_set_clip_rectangle(rectangle_information->light_gc, &event->area);
  gdk_gc_set_clip_rectangle(rectangle_information->dark_gc, &event->area);

  gdk_draw_line(display->bin_window, rectangle_information->dark_gc,
		rect.x_position + 1,
		rect.y_position + rect.height - 1,
		rect.x_position + rect.width - 2,
		rect.y_position + rect.height - 1);
  gdk_draw_line(display->bin_window, rectangle_information->dark_gc,
		rect.x_position + rect.width - 1,
		rect.y_position,
		rect.x_position + rect.width - 1,
		rect.y_position + rect.height - 1);

  gdk_draw_line(display->bin_window, rectangle_information->light_gc,
		rect.x_position,
		rect.y_position,
		rect.x_position + rect.width - 2,
		rect.y_position);
  gdk_draw_line(display->bin_window, rectangle_information->light_gc,
		rect.x_position,
		rect.y_position,
		rect.x_position,
		rect.y_position + rect.height - 1);
}

/**
 * This sets up a drawing area covering a specified area, which will be
 * drawn by the render_fill_area_event() whenever it is needed.
 *
 * @param area The area which should be filled. 
 * @param display A pointer to the GtkLayout widget to draw on.
 * @param colour_value A 32-bit colour value with the colour to fill with.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_rectangle(struct layout_rectangle rect, GtkLayout *display,
			 uint32_t light_colour_value, 
			 uint32_t dark_colour_value)
{
  GdkGC *light_gc, *dark_gc, *background_gc;
  GdkColor light_colour, dark_colour, background_colour;
  uint32_t *bg_colourp;
  struct gtkui_rectangle_information *rectangle_information;

  light_gc = gdk_gc_new(display->bin_window);
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), light_colour_value, 
				&light_colour);
  gdk_gc_set_foreground(light_gc, &light_colour);
  dark_gc = gdk_gc_new(display->bin_window);
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), dark_colour_value, 
				&dark_colour);
  gdk_gc_set_foreground(dark_gc, &dark_colour);

  /* Get the current background colour from the display. */
  bg_colourp = gtk_object_get_data(GTK_OBJECT(display), "background_colour");
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), *bg_colourp, 
				&background_colour);
  background_gc = gdk_gc_new(display->bin_window);
  gdk_gc_set_foreground(background_gc, &background_colour);

  /* !!! Memory leak !!! */

  rectangle_information = (struct gtkui_rectangle_information *)
    malloc(sizeof(struct gtkui_rectangle_information));
  if(rectangle_information == NULL)
    return 1;
  
  rectangle_information->light_gc = light_gc;
  rectangle_information->dark_gc = dark_gc;
  rectangle_information->background_gc = background_gc;
  rectangle_information->rectangle = rect;

  gtk_signal_connect(GTK_OBJECT(display), "expose_event",
		     (GtkSignalFunc)render_rectangle_event, 
		     rectangle_information);

  return 0;
}

/**
 *
 */
static void cleanup_display(GtkWidget *widget, gpointer data)
{
  /* All signal handlers should be removed. */
  gtk_signal_handlers_destroy(GTK_OBJECT(widget));

  /* Clean up all children, if any. */
  if(GTK_IS_CONTAINER(widget)) {
    gtk_container_foreach(GTK_CONTAINER(widget), 
			  (GtkCallback)cleanup_display, NULL);
  }

  /* Destroy the actual widget. */
  gtk_widget_destroy(widget);
}

/**
 * Render the page by calling the appropriate rendering functions. This is 
 * only done once for each page in the GTK+ user interface. 
 *
 * @param parts A pointer to the first layout part in the list to be rendered.
 * @param display A pointer to the GtkLayout widget to render on. If this is
 * @param display NULL, then the display area associated with the whole page
 * @param display is used. 
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_render(struct layout_part *parts, GtkLayout *display)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  struct layout_part *partp;
  struct gtkui_page_information *page_information;
  uint32_t *colourp;
  GdkColor colour;
  GtkAdjustment *adjustment;

  partp = parts;
  
  if(display == NULL)
    display = info->display;

  while(partp) {

    /*
    fprintf(stderr, 
	    "%s: partp->type=%d, partp->child=%p, partp->x=%d, partp->y=%d, partp->w=%d, partp->h=%d\n",
	    __FUNCTION__, partp->type, partp->child, 
	    partp->geometry.x_position, partp->geometry.y_position,
	    partp->geometry.width, partp->geometry.height);
    */

    /* Only the supported parts are represented here, all the others are
     * covered by the default statement, which does not do anything.
     */
    switch(partp->type) {
    case LAYOUT_PART_PAGE_INFORMATION:
      info->default_background_colour = 
	partp->data.page_information.background_colour;
      info->default_text_colour = 
	partp->data.page_information.text_colour;
      info->default_link_colour = 
	partp->data.page_information.link_colour;
      info->default_active_link_colour = 
	partp->data.page_information.active_link_colour;
      info->default_visited_link_colour = 
	partp->data.page_information.visited_link_colour;

      /* Hide all old things. */
      gtk_widget_hide_all(GTK_WIDGET(display));

      /* All signal handlers should be removed. */
      gtk_signal_handlers_destroy(GTK_OBJECT(display));

      /* Make sure all the previous widgets on the display are removed. */
      gtk_container_foreach(GTK_CONTAINER(display), 
			    (GtkCallback)cleanup_display, NULL);

      /* Put the default background colour of this display as an association
       * in the display widget. The allocated memory for the colour is freed
       * automatically when the widget is destroyed. 
       */
      colourp = (uint32_t *)malloc(sizeof(uint32_t));
      *colourp = info->default_background_colour;
      gtk_object_set_data_full(GTK_OBJECT(display), "background_colour",
			       colourp, (GtkDestroyNotify)free);

      /* Reset scrolling to the upper left corner. */
      adjustment = gtk_layout_get_vadjustment(display);
      gtk_adjustment_set_value(adjustment, 0);
      adjustment = gtk_layout_get_hadjustment(display);
      gtk_adjustment_set_value(adjustment, 0);

      /* Set the correct size of the whole display area. */
      gtk_layout_set_size(display, 
			  partp->geometry.width, partp->geometry.height);
      gtk_layout_freeze(display);

      gtkui_fill_area(partp->geometry, display, 
		      info->default_background_colour);
      
      gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), *colourp, &colour);
      gdk_gc_set_background(GTK_WIDGET(display)->style->
			    fg_gc[GTK_STATE_NORMAL], &colour);
      gdk_gc_set_background(GTK_WIDGET(display)->style->
			    text_gc[GTK_STATE_NORMAL], &colour);
      /*
      gdk_gc_set_foreground(GTK_WIDGET(display)->style->
			    bg_gc[GTK_STATE_PRELIGHT], &colour);
      */

      gtkui_set_title(partp->data.page_information.title);
      gtkui_set_url(partp->data.page_information.url);

      break;

    case LAYOUT_PART_TEXT:
      gtkui_draw_text(partp, display);
      break;

    case LAYOUT_PART_GRAPHICS:
      gtkui_draw_image(partp, display);
      break;

    case LAYOUT_PART_LINE:
      gtkui_draw_line(partp, display);
      break;

    case LAYOUT_PART_TABLE:
      gtkui_draw_table(partp, display);
      break;

    case LAYOUT_PART_FORM_SUBMIT:
      gtkui_draw_form_submit(partp, display);
      break;

    case LAYOUT_PART_FORM_CHECKBOX:
      gtkui_draw_form_checkbox(partp, display);
      break;

    case LAYOUT_PART_FORM_RADIO:
      gtkui_draw_form_radio(partp, display);
      break;

    case LAYOUT_PART_FORM_TEXT:
      gtkui_draw_form_text(partp, display);
      break;

    default:
      break;
    }

    if(partp->type != LAYOUT_PART_TABLE)
      gtkui_render(partp->child, display);
    if(partp->type == LAYOUT_PART_PAGE_INFORMATION) {
      gtk_layout_thaw(display);
          
      /* Reset the adjustments of the display, so the scrollbar
       * positions are restored.
       */
      page_information = 
	(struct gtkui_page_information *)partp->interface_data;
      if(page_information != NULL) {
	gtk_adjustment_set_value(gtk_layout_get_hadjustment(display), 
				 page_information->hscroll);
	gtk_adjustment_set_value(gtk_layout_get_vadjustment(display), 
				 page_information->vscroll);
      }

      /* Show all new things. */
      gtk_widget_show_all(GTK_WIDGET(display));

      break;
    }

    partp = partp->next;
  }

  return 0;
}

