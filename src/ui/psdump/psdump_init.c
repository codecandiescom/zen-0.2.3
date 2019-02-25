/**
 * Contains functions to initialize the Postscript dump user interface.
 *
 * @author Magnus Henoch <mange@freemail.hu>
 */

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#include "zen_ui.h"
#include "version.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/* A pointer to the user interface struct. This is global because
 * the other parts of the psdump user interface will want to get some
 * information from this.
 */
struct zen_ui *psdump_ui;

/**
 * Initialize the Postscript dump user interface. This is the function
 * that the main program will look for after loading the psdump shared
 * library. It sets up the zen_ui struct with appropriate values and
 * sets the zen_ui_operations struct to point to the dump functions. 
 *
 * @param argc The number of arguments given on the command line.
 * @param argv An array of arguments given on the command line. 
 * @param ui A zen_ui struct that contains information about the
 * @param ui psdump user interface. 
 *
 * @return non-zero value if an error occurred.
 */
int init(int argc, char *argv[], struct zen_ui *ui)
{
  static struct zen_ui_operations psdump_operations = {
    psdump_open,
    psdump_close,
    psdump_set_size,
    NULL
  };
  static struct psdump_information psdump_specific;

  ui->ui_type = ZEN_UI_GRAPHICAL;
  ui->ui_operations = &psdump_operations;
  ui->ui_name = UI_NAME;
  ui->ui_version_text = UI_VERSION_TEXT;
  ui->ui_version_number = UI_VERSION_NUMBER;

  /* Table support is optional; setting "table_support" checked in psdump_open */
  ui->ui_support.table = 1;
  ui->ui_support.form = 0;
  ui->ui_support.frame = 0;
  ui->ui_support.image = 1;
  ui->ui_support.font = 1;
  ui->ui_support.interaction = 0;
  /* to ensure page splitting at correct places: */
  ui->ui_support.freemove = 0;
  ui->ui_support.scrollable_x = 0;
  ui->ui_support.scrollable_y = 0;

  ui->ui_settings.min_fontsize = 8;
  ui->ui_settings.max_fontsize = 36;
  ui->ui_settings.default_fontsize = 10;

  /* Postscript coordinates are measured in points. 1 point =
   * 1/72 inch ~ 1/28 cm. (0, 0) is the bottom left of the paper.
   * 
   * psdump_page_size can be set to a number of predefined page sizes,
   * e.g. "a4", "letter". If it is not recognised, psdump_page_width and
   * psdump_page_height are taken to be page size in points. The defaults
   * are for A4. Then there are psdump_{left,right,top,bottom}_margin.
   *
   * Right now, I map Zen pixels to Postscript points as 1:1.
   *
   * These variables are set in psdump_open:
   *  ui->ui_display.width
   *  ui->ui_display.height
   *  psdump_specific.left_margin
   *  psdump_specific.bottom_margin
   */

  /* psdump_page_size is not set by default; otherwise it would be
   * impossible to give a custom page size
   */
  ui->ui_functions->set_setting("psdump_page_size", (void*)"", SETTING_STRING);
  ui->ui_functions->set_setting("psdump_page_width", (void*)595, SETTING_NUMBER);
  ui->ui_functions->set_setting("psdump_page_height", (void*)842, SETTING_NUMBER);
  ui->ui_functions->set_setting("psdump_left_margin", (void*)72, SETTING_NUMBER);
  ui->ui_functions->set_setting("psdump_right_margin", (void*)72, SETTING_NUMBER);
  ui->ui_functions->set_setting("psdump_top_margin", (void*)72, SETTING_NUMBER);
  ui->ui_functions->set_setting("psdump_bottom_margin", (void*)72, SETTING_NUMBER);

  /* Bitdepth... this is a tough one. Postscript colours are
   * specified as three decimal numbers between 0 and 1, so
   * these values are just virtual.
   */
  ui->ui_display.bit_depth = 24;
  ui->ui_display.red_length = 8;
  ui->ui_display.green_length = 8;
  ui->ui_display.blue_length = 8;
  ui->ui_display.red_offset = 16;
  ui->ui_display.green_offset = 8;
  ui->ui_display.blue_offset = 0;
  ui->ui_display.endian = ZEN_UI_DISPLAY_BIG_ENDIAN;  

  psdump_specific.outfile = stdout;
/*   psdump_specific.default_background_colour = 0xc0c0c0; */
  psdump_specific.default_text_colour = 0x000000;
  psdump_specific.default_link_colour = 0x0000dd;
  psdump_specific.default_active_link_colour = 0xdd0000;
  psdump_specific.default_visited_link_colour = 0xdd00dd;
  psdump_specific.y_distance_printed = 0;
  psdump_specific.lastcolour = 0xff000000;
  psdump_specific.lastfont[0] = '\0';
  psdump_specific.lastfontsize = 0;
  psdump_specific.page_number = 1;

  /* Set the pointer to the generic interface area. */
  ui->ui_specific = &psdump_specific;

  psdump_ui = ui;

  return 0;
}
