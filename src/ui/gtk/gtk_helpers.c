/**
 * Functions to help the GTK+ user interface on its course into perfection.
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
#include <sys/types.h>
#include <gtk/gtk.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * Set the width and height of a part, which only the user interface
 * can know the width and height of. This includes text strings and
 * various widgets in forms.
 *
 * @param partp A pointer to the layout part to set the size of.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_set_size(struct layout_part *partp)
{
  /* Make sure we do not let others mess with GTK+ while we
   * are doing it here.
   */
  gdk_threads_enter();

  switch(partp->type) {
  case LAYOUT_PART_TEXT:
    {
      GdkFont *font;

      font = gtkui_get_font(partp);
/*        font = gdk_font_ref(font); */
      partp->geometry.width = gdk_string_width(font, partp->data.text.text);
      partp->geometry.height = font->ascent + font->descent + 1;
/*        gdk_font_unref(font); */
    }
    break;

  case LAYOUT_PART_FORM_SUBMIT:
    {
      GtkWidget *button, *window;

#ifdef DEBUG
      fprintf(stderr, "%s(%d): submit value='%s'\n",
	      __FUNCTION__, __LINE__, partp->data.form_submit.value);
#endif /* DEBUG */

      window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      button = gtk_button_new_with_label(partp->data.form_submit.value);
      gtk_widget_show(button);
      gtk_container_add(GTK_CONTAINER(window), button);
      gtk_widget_realize(window);
      partp->geometry.width = button->allocation.width + 2;
      partp->geometry.height = button->allocation.height;

#ifdef DEBUG
      fprintf(stderr, "%s(%d): submit width=%d, height=%d\n",
	      __FUNCTION__, __LINE__, 
	      partp->geometry.width, partp->geometry.height);
#endif /* DEBUG */

      gtk_container_foreach(GTK_CONTAINER(window), 
			    (GtkCallback)gtk_widget_destroy, NULL);
      gtk_widget_destroy(window);
    }
    break;

  case LAYOUT_PART_FORM_CHECKBOX:
    {
      GtkWidget *checkbox, *window;

      window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      checkbox = gtk_check_button_new();
      gtk_widget_show(checkbox);
      gtk_container_add(GTK_CONTAINER(window), checkbox);
      gtk_widget_realize(window);
      partp->geometry.width = checkbox->allocation.width + 2;
      partp->geometry.height = checkbox->allocation.height;

#ifdef DEBUG
      fprintf(stderr, "%s(%d): checkbox width=%d, height=%d\n",
	      __FUNCTION__, __LINE__, 
	      partp->geometry.width, partp->geometry.height);
#endif /* DEBUG */

      gtk_container_foreach(GTK_CONTAINER(window), 
			    (GtkCallback)gtk_widget_destroy, NULL);
      gtk_widget_destroy(window);
    }
    break;

  case LAYOUT_PART_FORM_RADIO:
    {
      GtkWidget *radio, *window;

      window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      radio = gtk_radio_button_new(NULL);
      gtk_widget_show(radio);
      gtk_container_add(GTK_CONTAINER(window), radio);
      gtk_widget_realize(window);
      partp->geometry.width = radio->allocation.width + 2;
      partp->geometry.height = radio->allocation.height;

#ifdef DEBUG
      fprintf(stderr, "%s(%d): radio width=%d, height=%d\n",
	      __FUNCTION__, __LINE__, 
	      partp->geometry.width, partp->geometry.height);
#endif /* DEBUG */

      gtk_container_foreach(GTK_CONTAINER(window), 
			    (GtkCallback)gtk_widget_destroy, NULL);
      gtk_widget_destroy(window);
    }
    break;

  case LAYOUT_PART_FORM_TEXT:
    {
      GtkWidget *entry, *window;
      GtkRequisition req;

      window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      entry = gtk_entry_new();
      gtk_widget_size_request(entry, &req);
      gtk_widget_set_usize(entry, partp->data.form_text.size * 8, req.height);
      gtk_widget_show(entry);
      gtk_container_add(GTK_CONTAINER(window), entry);
      gtk_widget_realize(window);
      partp->geometry.width = entry->allocation.width + 2;
      partp->geometry.height = entry->allocation.height;

#ifdef DEBUG
      fprintf(stderr, "%s(%d): entry width=%d, height=%d\n",
	      __FUNCTION__, __LINE__, 
	      partp->geometry.width, partp->geometry.height);
#endif /* DEBUG */

      gtk_container_foreach(GTK_CONTAINER(window), 
			    (GtkCallback)gtk_widget_destroy, NULL);
      gtk_widget_destroy(window);
    }
    break;

  default:
    break;
  }

  /* Ok, let the others in on the fun again. */
  gdk_threads_leave();

  return 0;
}

/**
 * Converts a 32-bit colour value into a GdkColor struct, and match it to
 * the colourmap belonging to the given widget. 
 *
 * @param widget A pointer to the widget where the colourmap should be
 * @param widget taken from to match the colour.
 * @param colour_value A 32-bit colour value.
 * @param colour A pointer to a GdkColor struct, where the colour will 
 * @param colour end up in. 
 */
void gtkui_colourvalue_to_gdkcolor(GtkWidget *widget, uint32_t colour_value, 
				   GdkColor *colour)
{
  colour->red   = ((colour_value >> 16) & 0xff) << 8;
  colour->green = ((colour_value >>  8) & 0xff) << 8;
  colour->blue  = ((colour_value      ) & 0xff) << 8;

  /* Find the closest matching colour in the colourmap. */
  gdk_colormap_alloc_color(widget->style->colormap, colour, FALSE, TRUE);
}

/**
 * Put the title of the page as the title of the window.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_set_title(char *title_text)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;

  if(title_text == NULL)
    gtk_window_set_title(info->window, "Zen");
  else
    gtk_window_set_title(info->window, title_text);

  return 0;
}

/**
 * Sets the location entry to contain the new URL. If the user decides
 * to change it, using his imaginative wisdom, so be it. We cannot
 * control her methods of living.
 *
 * @param url The URL of the page.
 * 
 * @return non-zero value if an error occurred.
 */
int gtkui_set_url(char *url)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;

  if(url == NULL)
    gtk_entry_set_text(info->location, "");
  else
    gtk_entry_set_text(info->location, url);

  return 0;
}

/**
 * Display a text in the status entry field on the page.
 *
 * @param text The status text to display.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_set_status_text(char *text)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;

  if(text == NULL)
    gtk_entry_set_text(info->status, "");
  else
    gtk_entry_set_text(info->status, text);

  return 0;
}

