/**
 * Open function for the GTK+ user interface.
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
#include <dlfcn.h>
#include <gtk/gtk.h>

#include "paths.h"
#include "progress_logo.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * This is called when the user presses the closing button of the window.
 * It will stop the gtk_main() function, and consequently, also the
 * gtkui_open() function, and the program will terminate.
 *
 * @param widget
 * @param event
 * @param data
 *
 * @return TRUE or FALSE.
 */
static gint delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
  gtk_main_quit();
  return FALSE;
}

/**
 * This is called when the user presses the exit button. It is virtually
 * the same as delete_event().
 * It will stop the gtk_main() function, and consequently, also the
 * gtkui_open() function, and the program will terminate.
 *
 * @param widget
 * @param data
 *
 * @return TRUE or FALSE.
 */
static void exit_window_event(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

/**
 * This event is called by GTK+ if the back button was clicked.
 * It renders the previous page in the history list. 
 *
 * @param button
 * @param data
 */
static void back_button_event(GtkButton *button, gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  struct gtkui_page_information *page_information;

  if(info->current_page && info->current_page->previous) {
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

    info->current_page = info->current_page->previous;
    gtkui_render(info->current_page, NULL);
    gtkui_set_status_text(NULL);
  }
}

/**
 * This event is called by GTK+ if the forward button was clicked.
 * It renders the next page in the history list. 
 *
 * @param button
 * @param data
 */
static void forward_button_event(GtkButton *button, gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  struct gtkui_page_information *page_information;

  if(info->current_page && info->current_page->next) {
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

    info->current_page = info->current_page->next;
    gtkui_render(info->current_page, NULL);
    gtkui_set_status_text(NULL);
  }
}

/**
 * This event is called by GTK+ if the reload button was clicked.
 * It retrieves the current page again.
 *
 * @param button
 * @param data
 */
static void reload_button_event(GtkButton *button, gpointer data)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  char *status_text;

  if(info->current_page) {
    /* Request the URL that the user clicked upon. */
    status_text = (unsigned char *)
      malloc(strlen(info->current_page->data.page_information.url) + 16);
    if(status_text == NULL)
      return;

    info->page_id = gtkui_ui->ui_functions->
      request_page(info->current_page->data.page_information.url, NULL); 
    sprintf(status_text, "Loading: %s", 
	    info->current_page->data.page_information.url);
    gtkui_set_status_text(status_text);

    free(status_text);
  }
}

/**
 *
 */
static gboolean show_animation_event(GtkWidget *widget, GdkEventExpose *event)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  GdkGC *gc;

  if(info->progress_image_pointer == 0) {
    gdk_draw_rectangle(widget->window, 
		       widget->style->bg_gc[GTK_STATE_NORMAL],
		       TRUE, 
		       event->area.x, event->area.y, 
		       event->area.width, event->area.height);
  }

  gc = gdk_gc_new(widget->window);

  gdk_gc_set_clip_mask(gc, info->progress_masks[info->progress_image_pointer]);
  
  gdk_draw_pixmap(widget->window, gc, 
		  info->progress_images[info->progress_image_pointer],
		  event->area.x, event->area.y, 
		  event->area.x, event->area.y, 
		  event->area.width, event->area.height);

  gdk_gc_destroy(gc);

  return TRUE;
}

/**
 * Create the animation at the upper right corner. It is stored in 
 * the gtkui_information struct, as an array of pointers to 
 * GtkPixmaps. If the files could not be found, the pointer to this 
 * array is set to NULL.
 */
static void create_progress_images(GtkWidget *window)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  int i, size;
  char *error;
  char *progress_logo_name;
  progress_logo_t *progress_logo_p;
  int *progress_logo_amount_p;
  void *dl_handle, *value;

  /* If something goes wrong with the shared library loading here, we
   * want to make sure the interface does not think there is a logo to
   * be drawn. We do this by first telling it, there are zero number of
   * images in the progess logo array, since there truly are none there.
   */
  info->progress_image_amount = 0;

  /* Here, we check if the user has specified that she does not want a 
   * progress logo at all, in which case, we just exit here without
   * further notice.
   */
  gtkui_ui->ui_functions->get_setting("gtk_use_progress_logo", &value);
  if(!(int)value)
    return;

  size = strlen(UIDIR) + 64;
  progress_logo_name = (char *)malloc(size);
  if(progress_logo_name == NULL)
    return;

  sprintf(progress_logo_name, "%s/libzen_progress_%s.so", UIDIR, "zen");
  dl_handle = dlopen(progress_logo_name, RTLD_NOW);

  if(dl_handle == NULL) {
    fprintf(stderr, "Warning! Could not open the progress logo library %s\n", 
	    progress_logo_name);
    fprintf(stderr, "Error: %s\n", dlerror());
    free(progress_logo_name);
    return;
  }

  free(progress_logo_name);

  progress_logo_p = dlsym(dl_handle, "progress_logo");
  progress_logo_amount_p = dlsym(dl_handle, "progress_logo_amount");
  error = dlerror();
  if(error != NULL) {
    fprintf(stderr, "%s\n", error);
    dlclose(dl_handle);
    return;
  }

  info->progress_image_amount = *progress_logo_amount_p;

  info->progress_images = (GdkPixmap **)malloc(sizeof(GdkPixmap **) * 
					       info->progress_image_amount);
  if(info->progress_images == NULL) {
    info->progress_image_amount = 0;
    dlclose(dl_handle);
    return;
  }
  info->progress_masks = (GdkBitmap **)malloc(sizeof(GdkBitmap **) * 
					      info->progress_image_amount);
  if(info->progress_masks == NULL) {
    info->progress_image_amount = 0;
    dlclose(dl_handle);
    return;
  }

  for(i = 0 ; i < info->progress_image_amount ; i++) {
    pixmap = gdk_pixmap_create_from_xpm_d(window->window, &mask, NULL, 
					  (*progress_logo_p)[i]);
    info->progress_images[i] = pixmap;
    info->progress_masks[i] = mask;
  }  

  /* We can close the progress logo library here, since we have now 
   * converted it into GDK pixmaps. 
   */
  dlclose(dl_handle);
}

/**
 * Build the window which makes out the display and the control buttons.
 * This does not show the created window, but does show all the
 * widgets inside it. That means the calling function must only
 * show the window to make everything visible. 
 *
 * @param width The width of the new window.
 * @param height The height of the new window.
 *
 * @return a pointer to the GtkWidget corresponding to the window.
 */
GtkWidget *gtkui_create_window(int width, int height)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  GtkWidget *window;
  GtkWidget *entry;
  GtkWidget *drawing_area = NULL;
  GtkWidget *vbox, *hbox, *tbox, *handle_box, *layout, *table;
  GtkWidget *button, *label, *status;
  GtkWidget *hscrollbar, *vscrollbar;
  GtkAdjustment *hadjustment, *vadjustment;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Zen");
  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
		     GTK_SIGNAL_FUNC(delete_event), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_window_set_default_size(GTK_WINDOW(window), width, height);

  gtk_widget_realize(window);
  /* Create the animation we have in the upper right corner. */
  create_progress_images(window);
  gtk_widget_unrealize(window);
  
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_widget_show(vbox);

  handle_box = gtk_handle_box_new();
  gtk_box_pack_start(GTK_BOX(vbox), handle_box, FALSE, TRUE, 0);
  gtk_widget_show(handle_box);

  tbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(handle_box), tbox);
  gtk_widget_show(tbox);

  hbox = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(tbox), hbox, FALSE, TRUE, 0);
  gtk_widget_show(hbox);

  button = gtk_button_new_with_label("Back");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(back_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label("Forward");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(forward_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label("Reload");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(reload_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label("Stop");
  /*
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(hello), "button 1");
  */
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
  gtk_widget_show(button);

  button = gtk_button_new_with_label("Exit");
  gtk_signal_connect(GTK_OBJECT(button), "clicked",
		     GTK_SIGNAL_FUNC(exit_window_event), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
  gtk_widget_show(button);

  if(info->progress_image_amount > 0) {
    drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), 32, 32);
    gtk_box_pack_end(GTK_BOX(tbox), drawing_area, TRUE, TRUE, 0);
    gtk_widget_show(drawing_area);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "expose_event",
		       (GtkSignalFunc)show_animation_event, NULL);
  }

  handle_box = gtk_handle_box_new();
  gtk_box_pack_start(GTK_BOX(vbox), handle_box, FALSE, TRUE, 0);
  gtk_widget_show(handle_box);

  status = gtk_entry_new();
  gtk_entry_set_editable(GTK_ENTRY(status), FALSE);
  gtk_widget_set_sensitive(status, FALSE);
  gtk_container_add(GTK_CONTAINER(handle_box), status);
  gtk_widget_show(status);

  handle_box = gtk_handle_box_new();
  gtk_box_pack_start(GTK_BOX(vbox), handle_box, FALSE, TRUE, 0);
  gtk_widget_show(handle_box);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(handle_box), hbox);
  gtk_widget_show(hbox);

  label = gtk_label_new("Location: ");
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, TRUE, 0);
  gtk_widget_show(label);

  entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
  gtk_widget_show(entry);

  table = gtk_table_new(2, 2, FALSE);
  gtk_box_pack_start (GTK_BOX(vbox), table, TRUE, TRUE, 0);
  gtk_widget_show(table);

  layout = gtk_layout_new(NULL, NULL);
  gtk_table_attach(GTK_TABLE(table), layout, 0, 1, 0, 1, 
		   GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_show(layout);

  vadjustment = gtk_layout_get_vadjustment(GTK_LAYOUT(layout));
  vadjustment->step_increment = 8;
  hadjustment = gtk_layout_get_hadjustment(GTK_LAYOUT(layout));
  hadjustment->step_increment = 8;

  vscrollbar = gtk_vscrollbar_new(vadjustment);
  gtk_table_attach(GTK_TABLE(table), vscrollbar, 1, 2, 0, 1,
		   GTK_SHRINK, GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_widget_show(vscrollbar);

  hscrollbar = gtk_hscrollbar_new(hadjustment);
  gtk_table_attach(GTK_TABLE(table), hscrollbar, 0, 1, 1, 2,
		   GTK_SHRINK | GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_widget_show(hscrollbar);

  /* Save pointers to some specific widgets which are used by other
   * parts of the GTK+ user interface. 
   */
  info->window = GTK_WINDOW(window);
  info->display = GTK_LAYOUT(layout);
  info->location = GTK_ENTRY(entry);
  info->status = GTK_ENTRY(status);
  if(info->progress_image_amount > 0) {
    info->progress_logo = GTK_DRAWING_AREA(drawing_area);
  }

  info->current_page = NULL;
  info->page_id = -1;
  info->progress_image_pointer = 0;

  return window;
}

/**
 * Open up a window and setup all the GTK widgets. Also create a 
 * GDK pixmap that will make out the actual web page display. 
 *
 * @param url The URL given on the command line for the program.
 *
 * @return non-zero value if an error occurred.
 */
int gtkui_open(char *url)
{
  struct gtkui_information *info = 
    (struct gtkui_information *)gtkui_ui->ui_specific;
  int i, r, g, b;
  GtkWidget *window;
  GdkVisual *visual;
  GdkColormap *colours;
  GdkColor colour;

  /* Perform the initializating of GTK+ and company. */
  g_thread_init(NULL);
  gtk_init(&info->init_argc, &info->init_argv);

  /* Initialize GDK's RGB functions. Always use a private colourmap. */
  gdk_rgb_init();
  gdk_rgb_set_min_colors(256);
  gtk_widget_set_default_visual(gdk_rgb_get_visual());
  gtk_widget_set_default_colormap(gdk_rgb_get_cmap());

  /* Create the main window and all the widgets to fill it with. */
  window = gtkui_create_window(768, 512);

  /* Now we can set the correct display values of the interface. */
  visual = gtk_widget_get_visual(window);

  gtkui_ui->ui_display.bit_depth = 24;
  gtkui_ui->ui_display.red_length = 8;
  gtkui_ui->ui_display.green_length = 8;
  gtkui_ui->ui_display.blue_length = 8;
  gtkui_ui->ui_display.red_offset = 16;
  gtkui_ui->ui_display.green_offset = 8;
  gtkui_ui->ui_display.blue_offset = 0;
  gtkui_ui->ui_display.endian = ZEN_UI_DISPLAY_BIG_ENDIAN;  

  /* Setup the current colourmap here. */
  if(visual->type == GDK_VISUAL_PSEUDO_COLOR ||
     visual->type == GDK_VISUAL_STATIC_COLOR) {
    colours = gtk_widget_get_colormap(window);
    if(colours == NULL) {
      return 1;
    }
    
    /* Set the defaulted palette. */
    i = 0;
    for(r = 0 ; r < 65536 ; r += 65535 / 5) {
      for(g = 0 ; g < 65536 ; g += 65535 / 5) {
	for(b = 0 ; b < 65536 ; b += 65535 / 5) {
	  /* For safety. */
	  if(i >= 216)
	    break;
	  colour.red = r;
	  colour.green = g;
	  colour.blue = b;
	  colour.pixel = i;
	  gdk_colormap_alloc_color(colours, &colour, FALSE, TRUE);
	  i++;
	}
      }
    }
    for(r = 65535 / 15 ; r < 65536 ; r += 65535 / 15) {
      if((r % (65535 / 5)) != 0) {
	colour.red = r;
	colour.green = 0;
	colour.blue = 0;
	colour.pixel = i;
	gdk_colormap_alloc_color(colours, &colour, FALSE, TRUE);
	i++;
      }
    }
    for(g = 65535 / 15 ; g < 65536 ; g += 65535 / 15) {
      if((g % (65535 / 5)) != 0) {
	colour.red = 0;
	colour.green = g;
	colour.blue = 0;
	colour.pixel = i;
	gdk_colormap_alloc_color(colours, &colour, FALSE, TRUE);
	i++;
      }
    }
    for(b = 65535 / 15 ; b < 65536 ; b += 65535 / 15) {
      if((b % (65535 / 5)) != 0) {
	colour.red = 0;
	colour.green = 0;
	colour.blue = b;
	colour.pixel = i;
	gdk_colormap_alloc_color(colours, &colour, FALSE, TRUE);
	i++;
      }
    }
    for(g = 65535 / 15 ; g < 65536 ; g += 65535 / 15) {
      if((g % (65535 / 5)) != 0) {
	colour.red = g;
	colour.green = g;
	colour.blue = g;
	colour.pixel = i;
	gdk_colormap_alloc_color(colours, &colour, FALSE, TRUE);
	i++;
      }
    }

    if(i != 256)
      fprintf(stderr, 
	      "Warning! Palette index went wrong and ended up at %d!\n", i);
  }

  /* Show the main, and so far only, window. */
  gtk_widget_show(window);

  /* Store the size of the display area, so that the rest of the program
   * also knows about this. 
   */
  gtkui_ui->ui_display.width = GTK_WIDGET(info->display)->allocation.width;
  gtkui_ui->ui_display.height = GTK_WIDGET(info->display)->allocation.height;

  /* Turn over control to our main event handler.
   * GTK+ has its own main event loop, but we want to do some other
   * things first, so we still have our own function for it.
   */
  gtkui_main(url);

  return 0;
}
