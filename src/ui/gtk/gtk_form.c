/**
 * Functions to render form widgets on the GTK+ user interface.
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
 *
 */
static void form_submit_event(GtkButton *button, gpointer data)
{
  struct layout_part *partp;
  
  partp = (struct layout_part *)gtk_object_get_data(GTK_OBJECT(button), 
						    "layout_part");

#ifdef DEBUG
  fprintf(stderr, "%s(%d): submit button name '%s' value '%s' clicked\n",
	  __FUNCTION__, __LINE__, 
	  partp->data.form_submit.name, partp->data.form_submit.value);
#endif /* DEBUG */
}

/**
 * Place a submit button widget on the display.
 *
 * @param partp A pointer to the layout part representing the text.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_form_submit(struct layout_part *partp, GtkLayout *display)
{
  GtkWidget *button;

#ifdef DEBUG
  fprintf(stderr, "%s(%d): submit at %d,%d\n", __FUNCTION__, __LINE__, 
	  partp->geometry.x_position + 1, 
	  partp->geometry.y_position);
#endif /* DEBUG */

  /* Create the button with the correct label and place it on the 
   * display area.
   */
  button = gtk_button_new_with_label(partp->data.form_submit.value);
  gtk_layout_put(display, button, 
		 partp->geometry.x_position + 1, 
		 partp->geometry.y_position);

  /* Associate this button with the layout part. */
  gtk_object_set_data(GTK_OBJECT(button), "layout_part", partp);

  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(form_submit_event), NULL);
  gtk_widget_show(button);

  return 0;
}

/**
 * Place a checkbox widget on the display.
 *
 * @param partp A pointer to the layout part representing the text.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_form_checkbox(struct layout_part *partp, GtkLayout *display)
{
  GtkWidget *checkbox;
  uint32_t *bg_colourp;
  GdkColor colour;

#ifdef DEBUG
  fprintf(stderr, "%s(%d): checkbox at %d,%d\n", __FUNCTION__, __LINE__, 
	  partp->geometry.x_position + 1, 
	  partp->geometry.y_position);
#endif /* DEBUG */

  /* Create the checkbox and place it on the display area. */
  checkbox = gtk_check_button_new();

  bg_colourp = gtk_object_get_data(GTK_OBJECT(display), "background_colour");
  gtkui_colourvalue_to_gdkcolor(GTK_WIDGET(display), *bg_colourp, &colour);
  /*
  gdk_gc_set_foreground(checkbox->style->bg_gc[GTK_STATE_NORMAL], &colour);
  gdk_gc_set_foreground(checkbox->style->bg_gc[GTK_STATE_ACTIVE], &colour);
  gdk_gc_set_foreground(checkbox->style->bg_gc[GTK_STATE_PRELIGHT], &colour);
  gdk_gc_set_foreground(checkbox->style->bg_gc[GTK_STATE_INSENSITIVE], &colour);
  */

  gtk_widget_show(checkbox);
  gtk_layout_put(display, checkbox, 
		 partp->geometry.x_position + 1, 
		 partp->geometry.y_position);

  /* Associate this button with the layout part. */
  gtk_object_set_data(GTK_OBJECT(checkbox), "layout_part", partp);

  /* Set the current state of the checkbox, depending on if the HTML page
   * has specified it to be checked or not.
   */
  if(partp->data.form_checkbox.checked)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), FALSE);

  return 0;
}

/**
 * Place a radio button widget on the display.
 *
 * @param partp A pointer to the layout part representing the text.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_form_radio(struct layout_part *partp, GtkLayout *display)
{
  GtkWidget *radio;

#ifdef DEBUG
  fprintf(stderr, "%s(%d): radio at %d,%d\n", __FUNCTION__, __LINE__, 
	  partp->geometry.x_position + 1, 
	  partp->geometry.y_position);
#endif /* DEBUG */

  /* Create the checkbox and place it on the display area. */
  radio = gtk_radio_button_new(NULL);
  gtk_layout_put(display, radio, 
		 partp->geometry.x_position + 1, 
		 partp->geometry.y_position);

  /* Associate this button with the layout part. */
  gtk_object_set_data(GTK_OBJECT(radio), "layout_part", partp);

  /* Set the current state of the radio button, depending on if the HTML 
   * page has specified it to be checked or not.
   */
  if(partp->data.form_radio.checked)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), FALSE);

  gtk_widget_show(radio);

  return 0;
}

/**
 * Place a text field widget on the display.
 *
 * @param partp A pointer to the layout part representing the text.
 * @param display A pointer to the GtkLayout widget to draw on.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_draw_form_text(struct layout_part *partp, GtkLayout *display)
{
  GtkWidget *entry;
  GtkRequisition req;

#ifdef DEBUG
  fprintf(stderr, "%s(%d): text at %d,%d\n", __FUNCTION__, __LINE__, 
	  partp->geometry.x_position + 1, 
	  partp->geometry.y_position);
#endif /* DEBUG */

  /* Create the checkbox and place it on the display area. */
  entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), partp->data.form_text.maxlength);
  gtk_widget_size_request(entry, &req);
  gtk_widget_set_usize(entry, partp->data.form_text.size * 8, req.height);
  if(partp->data.form_text.value)
    gtk_entry_set_text(GTK_ENTRY(entry), partp->data.form_text.value);
  gtk_layout_put(display, entry, 
		 partp->geometry.x_position + 1, 
		 partp->geometry.y_position);

  /* Associate this button with the layout part. */
  gtk_object_set_data(GTK_OBJECT(entry), "layout_part", partp);

  gtk_widget_show(entry);

  return 0;
}

