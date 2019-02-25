/**
 * Functions for the main event loop in the GTK+ user interface.
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

/* For thread_yield function */
#include "threads.h"

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * This function is called with regular intervals from the GTK+ main
 * event loop. It will check for new status messages from the main
 * program, and will poll for a new page, if one has been requested.
 * If a new page is ready, it will call the rendering function.
 *
 * @param data A pointer to arbitrary data passed when registering
 * @param data this interval check routine.
 *
 * @return TRUE if the interval check should be kept, and FALSE if it
 * @return should end at this point.
 */
static gint interval_check(gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  struct layout_part *partp;
  struct gtkui_page_information *page_information;
  char status_text[2048];
  int last_progress_image_pointer;
  GdkEventExpose event_expose;
  gboolean *return_code;
  int error;

  thread_yield();

  if(gtkui_ui->ui_functions->get_status(info->page_id, status_text, 2047) > 0)
    gtkui_set_status_text(status_text);    
  
  /* Check if we have a new page ready for us. */
  error = gtkui_ui->ui_functions->poll_page(info->page_id, &partp);
  if(error < 0) {
    /* There was no new page, and there was an error somewhere. 
     * Possibly because the page was not found, or something even worse.
     * We then want to stop the progress logo animation. 
     */
    info->page_id = -1;
  } else if(partp != NULL) {
    /* Save the adjustment settings from the current page, before
     * rendering the new one. Then, when someone presses the Back
     * button, it will be restored.
     */
    if(info->current_page != NULL) {
      page_information = 
	(struct gtkui_page_information *)info->current_page->interface_data;
      page_information->hscroll = 
	gtk_layout_get_hadjustment(info->display)->value;
      page_information->vscroll = 
	gtk_layout_get_vadjustment(info->display)->value;
    }

    info->current_page = partp;
    gtkui_render(partp, NULL);
    gtkui_set_status_text(NULL);
    info->page_id = -1;

    page_information = (struct gtkui_page_information *)
      malloc(sizeof(struct gtkui_page_information));
    if(page_information == NULL)
      return FALSE;
    info->current_page->interface_data = (void *)page_information;
    info->current_page->free_interface_data = gtkui_free_interface_data;
  }

  /* We only want to deal with the progress logo things, if there 
   * actually is a progress logo to be drawn. 
   */
  if(info->progress_image_amount > 0) {
    /* Decide which progress animation image to draw. */
    last_progress_image_pointer = info->progress_image_pointer;
    if(info->page_id == -1) {
      info->progress_image_pointer = info->progress_image_amount - 1;
    } else {
      info->progress_image_pointer++;
      info->progress_image_pointer %= info->progress_image_amount;
    }
    
    /* Tell the logo to redraw itself, if it was changed. */
    if(last_progress_image_pointer != info->progress_image_pointer) {
      event_expose.type = GDK_EXPOSE;
      event_expose.window = GTK_WIDGET(info->progress_logo)->window;
      event_expose.send_event = TRUE;
      event_expose.area.x = 0;
      event_expose.area.y = 0;
      event_expose.area.width = 32;
      event_expose.area.height = 32;
      event_expose.count = 0;    
      
      if(gtk_signal_n_emissions_by_name(GTK_OBJECT(info->progress_logo),
					"expose_event") == 0) {
	gtk_signal_emit_by_name(GTK_OBJECT(info->progress_logo), 
				"expose_event", &event_expose, &return_code);
      }
    }
  }
    
  return TRUE;
}

/**
 * This is called when the user presses Return in the location entry box. 
 * The interface will then request a new page, and wait for that.
 *
 * @param widget
 * @param data
 */
static void activate_location(GtkWidget *widget, gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  char status_text[2048], *url;

  url = gtk_entry_get_text(info->location);

  info->page_id = gtkui_ui->ui_functions->request_page(url, NULL);
  sprintf(status_text, "Loading: %s", url);
  gtkui_set_status_text(status_text);
}

/**
 * This is the main event loop for the GTK+ user interface. Most of this
 * is controlled by GTK+, so I do not have to do much myself. That feels
 * good. 
 *
 * @param url The URL given at the command line.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_main(char *url)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  char status_text[2048];
  gint timeout_tag;

  /* Request a page, if it was given to us on the command line. */
  info->page_id = gtkui_ui->ui_functions->request_page(url, NULL);
  sprintf(status_text, "Loading: %s", url);
  gtkui_set_status_text(status_text);

  /* Register the interval checking routine, which will take care of
   * polling for new pages, look for new status texts and so on.
   * We do this check every 42 millisecond. 
   */
  timeout_tag = gtk_timeout_add(42, interval_check, NULL);

  gtk_signal_connect(GTK_OBJECT(info->location), "activate",
		     GTK_SIGNAL_FUNC(activate_location), NULL);
  
  /* Call the main event loop of the GTK+. This does it all! */
  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  gtk_timeout_remove(timeout_tag);

  return 0;
}
