/**
 * Open function for the Postscript dump user interface.
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
#include <string.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

struct page_size {
  char *name;
  int width;
  int height;
};

static struct page_size pagesizes[] = {
  { "Folio", 595, 935 }, 
  { "Executive", 522, 756 }, 
  { "Letter", 612, 792 }, 
  { "Legal", 612, 1008 }, 
  { "Ledger", 1224, 792 }, 
  { "Tabloid", 792, 1224 }, 
  { "A0", 2384, 3370 }, 
  { "A1", 1684, 2384 }, 
  { "A2", 1191, 1684 }, 
  { "A3", 842, 1191 }, 
  { "A4", 595, 842 }, 
  { "A5", 420, 595 }, 
  { "A6", 297, 420 }, 
  { "A7", 210, 297 }, 
  { "A8", 148, 210 }, 
  { "A9", 105, 148 }, 
  { "B0", 2920, 4127 }, 
  { "B1", 2064, 2920 }, 
  { "B2", 1460, 2064 }, 
  { "B3", 1032, 1460 }, 
  { "B4", 729, 1032 }, 
  { "B5", 516, 729 }, 
  { "B6", 363, 516 }, 
  { "B7", 258, 363 }, 
  { "B8", 181, 258 }, 
  { "B9", 127, 181 }, 
  { "B10", 91, 127 }
};


/**
 * This function will receive the URL given on the command line for the
 * program. It gets that page and does what it should.
 *
 * @param url The URL given on the command line for the program.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_open(char *url)
{
  struct psdump_information *info = 
    (struct psdump_information *)psdump_ui->ui_specific;
  struct layout_part *parts;
  void *settings_tmp;
  char *string_setting_tmp;
  int i, found;
  int page_width, page_height;
  int left_margin, top_margin, right_margin, bottom_margin;

  if (psdump_ui->ui_functions->get_setting("table_support", &settings_tmp) == SETTING_BOOLEAN) {
    psdump_ui->ui_support.table = (int)settings_tmp;
  }

  psdump_ui->ui_functions->get_setting("psdump_page_size", &settings_tmp);
  string_setting_tmp = (char*)settings_tmp;
  found = 0;
  for (i = 0; i < (sizeof pagesizes) / (sizeof pagesizes[0]); i++) {
    if (!strcasecmp(string_setting_tmp, pagesizes[i].name)) {
      found = 1;
      page_width = pagesizes[i].width;
      page_height = pagesizes[i].height;
      break;
    }
  }

  if (!found) {
    psdump_ui->ui_functions->get_setting("psdump_page_width", (void**)&page_width);
    psdump_ui->ui_functions->get_setting("psdump_page_height", (void**)&page_height);
  }

  psdump_ui->ui_functions->get_setting("psdump_left_margin", (void**)&left_margin);
  psdump_ui->ui_functions->get_setting("psdump_right_margin", (void**)&right_margin);
  psdump_ui->ui_functions->get_setting("psdump_top_margin", (void**)&top_margin);
  psdump_ui->ui_functions->get_setting("psdump_bottom_margin", (void**)&bottom_margin);

  /* We have the settings, now calculate values */
  psdump_ui->ui_display.width = page_width - left_margin - right_margin;
  psdump_ui->ui_display.height = page_height - top_margin - bottom_margin;
  info->left_margin = left_margin;
  info->bottom_margin = bottom_margin;

  /* Get what is on the URL that was handed to us. */
  parts = psdump_ui->ui_functions->get_page(url, NULL);
  if(parts == NULL)
    return 1;

  /* Render whatever was layouted from the URL. */
  psdump_render(parts);

  return 0;
}
