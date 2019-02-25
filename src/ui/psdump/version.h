/** 
 * Defines that contain the name and version of the Postscript dump
 * user interface.
 *
 * @author Magnus Henoch <mange@freemail.hu>
 */

#ifndef _UI_PSDUMP_VERSION_H_
#define _UI_PSDUMP_VERSION_H_

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

#define UI_NAME "Postscript dump interface"
#define UI_SHORT_NAME "psdump"
#define UI_VERSION_TEXT "0.0.1"
#define UI_VERSION_MAJOR 0
#define UI_VERSION_MINOR 0
#define UI_VERSION_MICRO 1
#define UI_VERSION_NUMBER (UI_VERSION_MAJOR * 262144 + \
			   UI_VERSION_MINOR * 512 + \
			   UI_VERSION_MICRO)

#endif /* _UI_PSDUMP_VERSION_H_ */
