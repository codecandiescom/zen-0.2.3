/**
 * Various information about the Postscript dump user interface.
 *
 * @author Magnus Henoch <mange@freemail.hu>
 */

#ifndef _UI_PSDUMP_INFO_H_
#define _UI_PSDUMP_INFO_H_

/*
 * Copyright (C) 2002, Magnus Henoch <mange@freemail.hu>
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
#include <stdio.h>

#include "zen_ui.h"

/**
 * Contains some generic information used by the psdump interface.
 *
 * @member outfile Where output is written, most likely stdout.
 * @member left_margin Left margin, in points
 * @member top_margin Bottom margin, in points
 * @member y_distance_printed Offset in Zen coordinates to get to the
 * @member y_distance_printed top of the current page
 * @member lastfont The last font used, saved to avoid duplicate specifications
 * @member lastfontsize The last font size used
 * @member lastcolour The last colour used; 0xff000000 means none
 * @member default_background_colour The background colour for the page.
 * @member default_text_colour The text colour for the page.
 * @member default_link_colour The link colour for the page.
 * @member default_active_link_colour The active link colour for the page.
 * @member default_visited_link_colour The visited link colour for the page.
 */
struct psdump_information {
  FILE *outfile;
  int left_margin;
  int bottom_margin;
  int y_distance_printed;
  int page_number;

  char lastfont[30];
  int lastfontsize;
  uint32_t lastcolour;

/*   uint32_t default_background_colour; */
  uint32_t default_text_colour;
  uint32_t default_link_colour;
  uint32_t default_active_link_colour;
  uint32_t default_visited_link_colour;
};

#if 0
/**
 * Holds information about the whereabouts of a link.
 *
 * @member link The geometry of the link. 
 * @member part A pointer to the actual layout part which represent the link.
 * @member visual_part A pointer to the layout part which is displayed.
 * @member next A pointer to the next link in the list.
 * @member previous A pointer to the previous link in the list.
 */
struct ofbis_link {
  struct layout_rectangle link;
  struct layout_part *part;
  struct layout_part *visual_part;
  struct ofbis_link *next;
  struct ofbis_link *previous;
};
#endif

extern struct zen_ui *psdump_ui;
extern int psdump_open(char *url);
extern int psdump_close(void);
extern int psdump_render(struct layout_part *partp);
#if 0
extern int psdump_set_title(char *title_text);
extern int psdump_set_url(char *url);
extern int psdump_set_status_text(char *text);
#endif
extern int psdump_draw_text(struct layout_part *partp);
extern int psdump_draw_image(struct layout_part *partp);
extern int psdump_draw_line(struct layout_part *partp);
extern int psdump_draw_table(struct layout_part *partp);
#if 0
extern int psdump_set_palette(int *red, int *green, int *blue);
#endif
extern int psdump_set_size(struct layout_part *partp);
extern int psdump_translate_x(int x);
extern int psdump_translate_y(int y);
extern int psdump_untranslate_width(double x);
extern void psdump_set_color(uint32_t colour);
extern void psdump_new_page(int next_y);
extern int psdump_choose_font(struct layout_part *partp,
			       char *fontnamebuffer);
/*extern int psdump_get_character_width(char *font, char c);*/
extern double psdump_get_string_width(char *string, char *fontname, int fontsize);

#if 0
extern int psdump_set_colour(int index, uint32_t colour);
extern int psdump_fill_area(struct layout_rectangle geometry, 
			   uint32_t colour);
extern int psdump_cut_rectangle(struct layout_rectangle part, 
			       struct layout_rectangle geometry,
			       struct layout_rectangle scroll,
			       struct layout_rectangle *cut);
extern void psdump_move_mouse(int x_distance, int y_distance, 
			     int init, int hide);
extern void psdump_create_links(struct layout_part *parts);
extern void psdump_activate_link(struct ofbis_link *linkp,
				struct layout_rectangle geometry,
				struct layout_rectangle scroll);

extern int psdump_main(char *url);
#endif

#endif /* _UI_PSDUMP_INFO_H_ */
