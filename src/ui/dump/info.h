/**
 * Various information about the dump user interface.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#ifndef _UI_DUMP_INFO_H_
#define _UI_DUMP_INFO_H_

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

#include "zen_ui.h"

extern struct zen_ui *dump_ui;
extern int dump_open(char *url);
extern int dump_close(void);
extern int dump_render(struct layout_part *partp);
extern int dump_set_size(struct layout_part *partp);
extern int dump_set_title(char *title_text);
extern int dump_draw_text(struct layout_part *partp);
extern int dump_draw_image(struct layout_part *partp);
extern int dump_draw_line(struct layout_part *partp);

#endif /* _UI_DUMP_INFO_H_ */
