/**
 * Various information about the GTK+ user interface.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#ifndef _UI_GTK_INFO_H_
#define _UI_GTK_INFO_H_

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

#include <sys/types.h>
#include <gtk/gtk.h>

#include "zen_ui.h"

/**
 * Contains some generic information used by the GTK+ interface.
 *
 * @member window A pointer to the widget representing the whole window.
 * @member display A pointer to the layout widget which makes out the
 * @member display area where the web page is rendered.
 * @member location A pointer to the entry widget which contains the current
 * @member location URL, or a new one, if the user has edited the field.
 * @member status A pointer to the entry widget which contains the current
 * @member status status message.
 * @member progress_logo A pointer to the widget containing the progress logo.
 * @member progress_images The progress logo images as GDK Pixmaps.
 * @member progress_masks The progress logo masks as GDK Pixmaps.
 * @member progress_image_amount The number of progress logo images.
 * @member progress_image_pointer The currently shown progress logo image.
 * @member default_background_colour The background colour for the page.
 * @member default_text_colour The text colour for the page.
 * @member default_link_colour The link colour for the page.
 * @member default_active_link_colour The active link colour for the page.
 * @member default_visited_link_colour The visited link colour for the page.
 * @member current_page A pointer to the currently displayed page.
 * @member page_id The identifier of the currently requested page. 
 */
struct gtkui_information {
  int init_argc;
  char **init_argv;
  GtkWindow *window;
  GtkLayout *display;
  GtkEntry *location;
  GtkEntry *status;
  GtkDrawingArea *progress_logo;
  GdkPixmap **progress_images;
  GdkBitmap **progress_masks;
  int progress_image_amount;
  int progress_image_pointer;
  uint32_t default_background_colour;
  uint32_t default_text_colour;
  uint32_t default_link_colour;
  uint32_t default_active_link_colour;
  uint32_t default_visited_link_colour;
  struct layout_part *current_page;
  int page_id;
};

/**
 * Holds information about the whereabouts of a link.
 *
 * @member link The geometry of the link. 
 * @member part A pointer to the actual layout part which represent the link.
 * @member visual_part A pointer to the layout part which is displayed.
 * @member next A pointer to the next link in the list.
 * @member previous A pointer to the previous link in the list.
 */
struct gtkui_link {
  struct layout_rectangle link;
  struct layout_part *part;
  struct layout_part *visual_part;
  struct gtkui_link *next;
  struct gtkui_link *previous;
};

/**
 * Data for the page layout part, which is specific for the GTK+ user
 * interface. 
 *
 * @member hscroll The horizontal scroll position of the page.
 * @member vscroll The vertical scroll position of the page.
 */
struct gtkui_page_information {
  gfloat hscroll;
  gfloat vscroll;
};

/**
 * Data for a filled area.
 *
 * @member gc The graphics context for this area.
 * @member area The area to fill.
 */
struct gtkui_fill_information {
  GdkGC *gc;
  struct layout_rectangle area;
};

/**
 * Data for a rectangle.
 *
 * @member light_gc The graphics context for the light sides.
 * @member dark_gc The graphics context for the dark sides.
 * @member background_gc The graphics context for the background.
 * @member rectangle The geometry of the rectangle.
 */
struct gtkui_rectangle_information {
  GdkGC *light_gc;
  GdkGC *dark_gc;
  GdkGC *background_gc;
  struct layout_rectangle rectangle;
};

/**
 * Data for the text layout parts, which is specific for the GTK+ user
 * interface. 
 *
 * @member font The GDK font used for this piece of text.
 * @member text_gc The graphics context for this text.
 * @member background_gc The graphics context for the background.
 * @member event_box The event box used if the text is a link, or NULL.
 */
struct gtkui_text_information {
  GdkFont *font;
  GdkGC *text_gc;
  GdkGC *background_gc;
  GtkEventBox *event_box;
};

/**
 * Data for the horizontal line layout parts, which is specific for the 
 * GTK+ user interface. 
 *
 * @member light_gc The graphics context for the light side of the line.
 * @member dark_gc The graphics context for the dark side of the line.
 * @member background_gc The graphics context for the background.
 */
struct gtkui_line_information {
  GdkGC *light_gc;
  GdkGC *dark_gc;
  GdkGC *background_gc;
};

/**
 * Data for the image layout parts, which is specific for the GTK+ user 
 * interface. 
 *
 * @member image_gc The graphics context for the image itself.
 * @member border_gc The graphics context for the border around the image.
 * @member background_gc The graphics context for the background.
 */
struct gtkui_image_information {
  GdkGC *image_gc;
  GdkGC *border_gc;
  GdkGC *background_gc;
};

extern struct zen_ui *gtkui_ui;
extern int gtkui_open(char *url);
extern int gtkui_close(void);
extern int gtkui_render(struct layout_part *partp, GtkLayout *display);
extern int gtkui_set_title(char *title_text);
extern int gtkui_set_url(char *url);
extern int gtkui_set_status_text(char *text);
extern int gtkui_draw_text(struct layout_part *partp, GtkLayout *display);
extern int gtkui_draw_image(struct layout_part *partp, GtkLayout *display);
extern int gtkui_draw_line(struct layout_part *partp, GtkLayout *display);
extern int gtkui_draw_table(struct layout_part *partp, GtkLayout *display);
extern int gtkui_draw_form_submit(struct layout_part *partp, 
				  GtkLayout *display);
extern int gtkui_draw_form_checkbox(struct layout_part *partp, 
				    GtkLayout *display);
extern int gtkui_draw_form_radio(struct layout_part *partp, 
				 GtkLayout *display);
extern int gtkui_draw_form_text(struct layout_part *partp, 
				GtkLayout *display);
extern int gtkui_set_size(struct layout_part *partp);
extern void gtkui_free_interface_data(struct layout_part *partp);

extern GdkFont *gtkui_get_font(struct layout_part *textp);
extern void gtkui_colourvalue_to_gdkcolor(GtkWidget *widget, 
					  uint32_t colour_value, 
					  GdkColor *colour);
extern int gtkui_set_colour(int index, uint32_t colour);
extern int gtkui_fill_area(struct layout_rectangle area, GtkLayout *display, 
			   uint32_t colour);
extern int gtkui_draw_rectangle(struct layout_rectangle rect, 
				GtkLayout *display,
				uint32_t light_colour_value, 
				uint32_t dark_colour_value);

extern int gtkui_main(char *url);

extern char *(*progress_logo[]);

#endif /* _UI_GTK_INFO_H_ */
