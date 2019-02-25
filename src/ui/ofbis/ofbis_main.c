/**
 * Functions for the main event loop in the oFBis user interface.
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
#include <string.h>
#include <stdlib.h>
#include <ofbis.h>

#include "layout.h"
#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/* The linked list of links. */
extern struct ofbis_link *links;

/* Current position of the mouse cursor. */
static int current_x_position = -1;
static int current_y_position = -1;

/* Current scrolling offset of the page. */
static struct layout_rectangle scroll;

/* Current geometry of the page. */
static struct layout_rectangle geometry;

/* The current active link being selected. */
static struct ofbis_link *active_link;

/**
 * Checks the given X and Y position against the parts stored in the
 * list of links, to see if the position is part of the link.
 *
 * @param x The X position of the point to check. If negative, use the
 * @param x globally stored current position.
 * @param y The Y position of the point to check. If negative, use the
 * @param y globally stored current position.
 *
 * @return a pointer to the link under the point, or NULL if there
 * @return was no link at the point.
 */
static struct ofbis_link *check_link(int x, int y)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  struct ofbis_link *linkp;
  int x_position, y_position;

  if(x < 0)
    x_position = current_x_position;
  else
    x_position = x;

  if(y < 0)
    y_position = current_y_position;
  else
    y_position = y;

  /* Compensate for scrolling and static part. */
  x_position += scroll.x_position - info->x_position;
  y_position += scroll.y_position - info->y_position;

  linkp = links;
  while(linkp) {
    if(x_position >= linkp->link.x_position &&
       x_position < (linkp->link.x_position + linkp->link.width) &&
       y_position >= linkp->link.y_position &&       
       y_position < (linkp->link.y_position + linkp->link.height))
      return linkp;

    linkp = linkp->next;
  }

  return NULL;
}

/**
 * Move the mouse cursor on the screen. This function keeps track of 
 * previous position of the mouse, and saves the background as well as
 * restoring it. If the mouse pointer stumbles over a link, it will
 * change the appearance of the mouse pointer slightly, and highlight
 * the link in the appropriate colour.
 *
 * @param x_distance The amount of pixels the mouse has been moved
 * @param x_distance horizontally since the last check.
 * @param y_distance The amount of pixels the mouse has been moved
 * @param y_distance vertically since the last check.
 * @param init Non-zero value means force reinitialization.
 * @param hide Non-zero value means hiding the pointer.
 */
void ofbis_move_mouse(int x_distance, int y_distance, int init, int hide)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  static int first = 1;
  FB *fb;
  FBBLTPBLK *fbb;
  static unsigned short *buffer = NULL;
  uint32_t colour;
  struct ofbis_link *linkp;

  /* Reinitialize the mouse, so that we do not mess up a newly
   * rendered page by restoring the background right upon it.  
   */
  if(init) {
    first = 1;
    if(buffer)
      free(buffer);
    buffer = NULL;
    return;
  }

  fb = info->ofbis_fb;

  if(first) {
    if(current_x_position < 0)
      current_x_position = ofbis_ui->ui_display.width / 2;
    if(current_y_position < 0)
      current_y_position = ofbis_ui->ui_display.height / 2;

    /* To save the trouble of checking exactly how much we need to
     * save, we just allocate some, more than we really need with
     * any known resolution. Of course, the time it has taken me
     * to write this comment, I could have written the correct formula
     * ten times over.
     */
    buffer = (unsigned short *)malloc(16 * 16 * 8);
    if(buffer == NULL)
      return;

    first = 0;
  } else {
    /* Restore old background. */
    fbb = FBgetbltpblk(fb);
    fbb->b_wd = 16;
    fbb->b_ht = 16;
    fbb->s_form = buffer;
    fbb->d_xmin = max(0, min(ofbis_ui->ui_display.width - 16, 
			     current_x_position - 8));
    fbb->d_ymin = max(0, min(ofbis_ui->ui_display.height - 16, 
			     current_y_position - 8));
    if(fb->finf.type == FB_TYPE_PACKED_PIXELS)
      fbb->s_nxln = 16 * ofbis_ui->ui_display.bit_depth / 8;
    else
      fbb->s_nxln = (fbb->b_wd + 15) / 16;
    fbb->s_nxpl = 2;
    FBbitblt(fb, fbb);
    FBfreebltpblk(fbb);
  }

  /* If we only want to hide the mouse, we do not need to do more than this.*/
  if(hide)
    return;

  /* Calculate the new X and Y position. */
  current_x_position = max(0, min(ofbis_ui->ui_display.width - 1, 
				  current_x_position + x_distance));
  current_y_position = max(0, min(ofbis_ui->ui_display.height - 1, 
				  current_y_position + y_distance));

  linkp = check_link(current_x_position, current_y_position);
  if(linkp) {
    active_link = linkp;
    ofbis_activate_link(active_link, geometry, scroll);
    colour = FBc24_to_cnative(fb, 0xff0000);
  } else {
    colour = FBc24_to_cnative(fb, 0x0000ff);
  }

  /* Save old background. */
  fbb = FBgetbltpblk(fb);
  fbb->b_wd = 16;
  fbb->b_ht = 16;
  fbb->s_xmin = max(0, min(ofbis_ui->ui_display.width - 16, 
			   current_x_position - 8));
  fbb->s_ymin = max(0, min(ofbis_ui->ui_display.height - 16, 
			   current_y_position - 8));
  fbb->d_form = buffer;
  if(fb->finf.type == FB_TYPE_PACKED_PIXELS)
    fbb->d_nxln = 16 * ofbis_ui->ui_display.bit_depth / 8;
  else
    fbb->d_nxln = (fbb->b_wd + 15) / 16;
  fbb->d_nxpl = 2;
  FBbitblt(fb, fbb);
  FBfreebltpblk(fbb);

  /* Draw cross. */
  FBhline(fb,
	  max(0, current_x_position - 6),
	  min(ofbis_ui->ui_display.width - 1, current_x_position + 6),
	  current_y_position,
	  colour);

  FBline(fb,
	 current_x_position,
	 max(0, current_y_position - 6),
	 current_x_position,
	 min(ofbis_ui->ui_display.height - 1, current_y_position + 6),
	 colour);

  if(linkp) {
    FBline(fb,
	   max(0, current_x_position - 4),
	   max(0, current_y_position - 4),
	   min(ofbis_ui->ui_display.width - 1, current_x_position + 4),
	   min(ofbis_ui->ui_display.height - 1, current_y_position + 4),
	   colour);

    FBline(fb,
	   min(ofbis_ui->ui_display.width - 1, current_x_position + 4),
	   max(0, current_y_position - 4),
	   max(0, current_x_position - 4),
	   min(ofbis_ui->ui_display.height - 1, current_y_position + 4),
	   colour);
  }  
}

/**
 * Scroll the page in some direction, and redraw the area which needs
 * to be redrawn in the new position.
 *
 * So far only scrolling in y direction has been tested.
 *
 * @param parts A pointer to the page information part of the page.
 * @param x_offset The offset in pixels to scroll in x direction. 
 * @param x_offset A negative value scrolls left, a positive right.
 * @param y_offset The offset in pixels to scroll in y direction. 
 * @param y_offset A negative value scrolls up, a positive down.
 *
 * @return non-zero value if an error occurred.
 */
static int ofbis_scroll_page(struct layout_part *parts, 
			     int x_offset, int y_offset)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;
  FBBLTPBLK *fbb;
  int real_scroll_x, real_scroll_y;
  struct layout_rectangle redraw_geometry, old_scroll, tmp_scroll;

  if(parts == NULL)
    return 1;

  fb = info->ofbis_fb;

  old_scroll = scroll;

  scroll.x_position += x_offset;
  if(scroll.x_position > parts->geometry.width - geometry.width)
    scroll.x_position = parts->geometry.width - geometry.width;
  if(scroll.x_position < 0)
    scroll.x_position = 0;

  scroll.y_position += y_offset;
  if(scroll.y_position > parts->geometry.height - geometry.height)
    scroll.y_position = parts->geometry.height - geometry.height;
  if(scroll.y_position < 0)
    scroll.y_position = 0;

  if(old_scroll.x_position != scroll.x_position || 
     old_scroll.y_position != scroll.y_position) {
    real_scroll_x = scroll.x_position - old_scroll.x_position;
    real_scroll_y = scroll.y_position - old_scroll.y_position;

    tmp_scroll = scroll;
    scroll = old_scroll;
    ofbis_move_mouse(0, 0, 0, 1);
    ofbis_move_mouse(0, 0, 1, 1);
    scroll = tmp_scroll;
        
    if(abs(real_scroll_y) < geometry.height) {
      fbb = FBgetbltpblk(fb);    
      fbb->b_wd = geometry.width;
      fbb->b_ht = geometry.height - abs(real_scroll_y);
      fbb->d_xmin = info->x_position;
      fbb->d_ymin = max(0, -real_scroll_y) + info->y_position;
      fbb->s_xmin = info->x_position;
      fbb->s_ymin = max(0, real_scroll_y) + info->y_position;
      FBbitblt(fb, fbb);
      FBfreebltpblk(fbb);
    }

    redraw_geometry.x_position = 0;
    if(real_scroll_y < 0)
      redraw_geometry.y_position = 0;
    else
      redraw_geometry.y_position = geometry.height - real_scroll_y;
    redraw_geometry.width = geometry.width;
    redraw_geometry.height = abs(real_scroll_y);

    /*
    fprintf(stderr, 
	    "rgeo.x=%d, rgeo.y=%d, rgeo.w=%d, rgeo.h=%d\n",
	    redraw_geometry.x_position, redraw_geometry.y_position,
	    redraw_geometry.width, redraw_geometry.height);
    */

    ofbis_render(parts, redraw_geometry, scroll);
  }

  return 0;
}

/**
 * Switch over to the tty and read a new URL.
 *
 * @param url The area where the new URL is stored.
 * @param max_length The maximum length of the string used for the URL.
 *
 * @return the actual number of characters read from the user.
 */
int ofbis_get_url_from_user(char *url, int max_length)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;

  fb = info->ofbis_fb;

  /* Switch to the original TTY. */
  FBVTswitchoriginal(fb);

  printf("Enter new URL: ");
  fflush(stdout);
  fgets(url, max_length, stdin);
  url[max_length] = '\0';
  if(strlen(url) > 0)
    if(url[strlen(url) - 1] == '\n')
      url[strlen(url) - 1] = '\0';

  /* Switch back to the framebuffer VT. */
  FBVTswitchfb(fb);

  return strlen(url);
}

/**
 * This is the main event loop for the oFBis user interface. This is where
 * it is all controlled. This is where the program take input from the user
 * and acts upon it. This is it.
 *
 * @param url The URL given at the command line.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_main(char *url)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  struct layout_part *parts = NULL, *tmp_part;
  FB *fb;
  FBEVENT event;
  struct ofbis_link *link;
  int button_pressed;
#ifdef USE_CONTROL_FIELD
  struct layout_rectangle control_geometry;
#endif
  char status_text[2048], *new_url;
  int page_id, error;

  fb = info->ofbis_fb;

#ifdef USE_CONTROL_FIELD
  control_geometry.x_position = 0;
  control_geometry.y_position = 0;
  control_geometry.width = ofbis_ui->ui_display.width;
  control_geometry.height = info->y_position - 1;
  ofbis_fill_area(control_geometry, info->control_field_colour);
#endif
  
  scroll.x_position = 0;
  scroll.y_position = 0;

  geometry.x_position = 0;
  geometry.y_position = 0;
  geometry.width = ofbis_ui->ui_display.width - info->x_position;
  geometry.height = ofbis_ui->ui_display.height - info->y_position;

  page_id = ofbis_ui->ui_functions->request_page(url, NULL);
  sprintf(status_text, "Loading: %s", url);
  ofbis_set_status_text(status_text);
  
  button_pressed = 0;
  active_link = NULL;
  while(1) {
    FBcheckevent(fb, &event, 42);

    if(ofbis_ui->ui_functions->get_status(page_id, status_text, 2047) > 0)
      ofbis_set_status_text(status_text);    

    /* Check if we have a new page ready for us. */
    error = ofbis_ui->ui_functions->poll_page(page_id, &tmp_part);
    if(error == 0 && tmp_part != NULL) {
      parts = tmp_part;
      scroll.x_position = 0;
      scroll.y_position = 0;
      ofbis_move_mouse(0, 0, 0, 1);
      ofbis_move_mouse(0, 0, 1, 1);
      ofbis_create_links(parts);
      ofbis_activate_link(NULL, geometry, scroll);
      ofbis_render(parts, geometry, scroll);
      active_link = NULL;
      ofbis_set_status_text(NULL);
    }

    switch(event.type) {
    case FBNoEvent:
      break;

    case FBKeyEvent:
      if(!(event.key.keycode & 0x80))
	break;

      switch(event.key.ascii) {
      case 'q':
	return 0;
	break;

      case 'g':
	ofbis_activate_link(NULL, geometry, scroll);
	active_link = NULL;
	new_url = (char *)malloc(2048);
	if(new_url == NULL)
	  return 1;
	if(ofbis_get_url_from_user(new_url, 2000) > 0) {
	  page_id = ofbis_ui->ui_functions->request_page(new_url, NULL);
	  sprintf(status_text, "Loading: %s", new_url);
	  ofbis_set_status_text(status_text);
	}
	free(new_url);
	break;

      case 'a':
	ofbis_scroll_page(parts, 0, -128);
	break;

      case 'A':
	ofbis_scroll_page(parts, 0, -1);
	break;

      case 'z':
	ofbis_scroll_page(parts, 0, 128);
	break;

      case 'Z':
	ofbis_scroll_page(parts, 0, 1);
	break;

      case 'x':
      case ' ':
	ofbis_scroll_page(parts, 0, geometry.height);
	break;

      case 's':
	ofbis_scroll_page(parts, 0, -geometry.height);
	break;

      case 'r':
	ofbis_activate_link(NULL, geometry, scroll);
	ofbis_render(parts, geometry, scroll);
	active_link = NULL;
	ofbis_set_status_text(NULL);
	break;

      case 'b':
	if(parts->previous) {
	  scroll.x_position = 0;
	  scroll.y_position = 0;
	  parts = parts->previous;
	  ofbis_create_links(parts);
	  ofbis_move_mouse(0, 0, 0, 1);
	  ofbis_move_mouse(0, 0, 1, 1);
	  ofbis_activate_link(NULL, geometry, scroll);
	  ofbis_render(parts, geometry, scroll);
	  active_link = NULL;
	  ofbis_set_status_text(NULL);
	}
	break;

      case 'f':
	if(parts->next) {
	  scroll.x_position = 0;
	  scroll.y_position = 0;
	  parts = parts->next;
	  ofbis_create_links(parts);
	  ofbis_move_mouse(0, 0, 0, 1);
	  ofbis_move_mouse(0, 0, 1, 1);
	  ofbis_activate_link(NULL, geometry, scroll);
	  ofbis_render(parts, geometry, scroll);
	  active_link = NULL;
	  ofbis_set_status_text(NULL);
	}
	break;

	/* These should really have been put in an oFBis header file. */
#define Mode_RShift 0x0001
#define Mode_LShift 0x0002
      case '\t':
	if(links == NULL)
	  break;

	if(event.key.state & (Mode_LShift | Mode_RShift)) {
	  if(active_link == NULL)
	    active_link = links;
	  else if(active_link->previous)
	    active_link = active_link->previous;
	} else {
	  if(active_link == NULL)
	    active_link = links;
	  else if(active_link->next)
	    active_link = active_link->next;
	}
	ofbis_activate_link(active_link, geometry, scroll);
	break;

      case '\n':
	if(active_link) {
	  page_id = ofbis_ui->ui_functions->
	    request_page(active_link->part->data.link.href,
			 parts->data.page_information.url);
	  sprintf(status_text, "Loading: %s", 
		  active_link->part->data.link.href);
	  ofbis_set_status_text(status_text);
	}
	break;

      }
      break;

    case FBMouseEvent:
      if(!button_pressed && !(event.mouse.buttons & 0x4)) {
	button_pressed = 1;
	ofbis_move_mouse(event.mouse.x, event.mouse.y, 0, 0);
      } else if(button_pressed && (event.mouse.buttons & 0x4)) {
	button_pressed = 0;
	link = check_link(-1, -1);
	if(link == NULL)
	  continue;

	page_id = ofbis_ui->ui_functions->
	  request_page(link->part->data.link.href,
		       parts->data.page_information.url);
	  sprintf(status_text, "Loading: %s", link->part->data.link.href);
	  ofbis_set_status_text(status_text);
      } else {
	ofbis_move_mouse(event.mouse.x, event.mouse.y, 0, 0);
      }
      break;
      
    default:
      break;
    }
  }

  return 0;
}
