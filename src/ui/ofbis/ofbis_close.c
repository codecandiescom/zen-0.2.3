/**
 * Close function for the oFBis user interface.
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
#include <ofbis.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Safely close down the oFBis user interface and its framebuffer.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_close(void)
{
  FB *fb = ((struct ofbis_information *)ofbis_ui->ui_specific)->ofbis_fb;

  if(ofbis_ui->ui_display.colourmap != NULL)
    free(ofbis_ui->ui_display.colourmap);

  /* In case we get to this point, without the framebuffer being open,
   * just return here from.
   */
  if(fb == NULL)
    return 1;

  FBclose(fb);

  return 0;
}
