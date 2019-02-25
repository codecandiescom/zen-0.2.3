/**
 * Declarations of the progress logos we have to choose from. So
 * far, there is only one. 
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#ifndef _UI_PROGRESS_LOGO_PROGRESS_LOGO_H_
#define _UI_PROGRESS_LOGO_PROGRESS_LOGO_H_

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

typedef char *(*progress_logo_t[]);

extern int progress_logo_amount;
extern progress_logo_t progress_logo;

#endif /* _UI_PROGRESS_LOGO_PROGRESS_LOGO_H_ */
