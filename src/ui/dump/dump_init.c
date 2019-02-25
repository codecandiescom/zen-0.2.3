/**
 * Contains functions to initialize the dump user interface.
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

#include "zen_ui.h"
#include "version.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/* A pointer to the user interface struct. This is global because
 * the other parts of the dump user interface will want to get some
 * information from this.
 */
struct zen_ui *dump_ui;

/**
 * Initialize the plain text dump user interface. This is the function
 * that the main program will look for after loading the dump shared
 * library. It sets up the zen_ui struct with appropriate values and
 * sets the zen_ui_operations struct to point to the dump functions. 
 *
 * @param argc The number of arguments given on the command line.
 * @param argv An array of arguments given on the command line. 
 * @param ui A zen_ui struct that contains information about the
 * @param ui dump user interface. 
 *
 * @return non-zero value if an error occurred.
 */
int init(int argc, char *argv[], struct zen_ui *ui)
{
  static struct zen_ui_operations dump_operations = {
    dump_open,
    dump_close,
    dump_set_size,
    NULL /* set_palette */
  };

  ui->ui_type = ZEN_UI_TEXTUAL;
  ui->ui_operations = &dump_operations;
  ui->ui_name = UI_NAME;
  ui->ui_version_text = UI_VERSION_TEXT;
  ui->ui_version_number = UI_VERSION_NUMBER;

  ui->ui_support.table = 0;
  ui->ui_support.form = 0;
  ui->ui_support.frame = 0;
  ui->ui_support.image = 0;
  ui->ui_support.font = 0;
  ui->ui_support.interaction = 0;
  ui->ui_support.freemove = 0;
  ui->ui_support.scrollable_x = 0;
  ui->ui_support.scrollable_y = 1;

  ui->ui_settings.min_fontsize = 8;
  ui->ui_settings.max_fontsize = 8;
  ui->ui_settings.default_fontsize = 8;

  /* Assume that the font is 8 pixels wide. I do not yet know a good way
   * of finding that out. It does not matter much, really.
   * An alternative would be to make is possible to set the resolution
   * as number of characters instead. I will have to think about that.
   */
  ui->ui_display.width = 80 * 8;

  /* There is no limit to the Y resolution in the ultimate dump user 
   * interface. 
   */
  ui->ui_display.height = -1;

  dump_ui = ui;

  return 0;
}
