/**
 * Open function for the dump user interface.
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

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * This function will receive the URL given on the command line for the
 * program. It gets that page and does what it should.
 *
 * @param url The URL given on the command line for the program.
 *
 * @return non-zero value if an error occurred.
 */
int dump_open(char *url)
{
  struct layout_part *parts;

  /* Get what is on the URL that was handed to us. */
  parts = dump_ui->ui_functions->get_page(url, NULL);
  if(parts == NULL)
    return 1;

  /* Render whatever was layouted from the URL. */
  dump_render(parts);

  return 0;
}
