/**
 * Close function for the Postscript dump user interface.
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
#include <stdlib.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/**
 * Safely close down the Postscript output.
 *
 * @return non-zero value if an error occurred.
 */
int psdump_close(void)
{
  struct psdump_information *info = (struct psdump_information *)psdump_ui->ui_specific;

  fprintf(info->outfile,
	  "\n"
	  "grestore\n"
	  "showpage\n"
	  "%%%%Trailer\n"
	  "%%%%Pages: %d\n"
	  "%%%%EOF\n", info->page_number);

  return 0;
}
