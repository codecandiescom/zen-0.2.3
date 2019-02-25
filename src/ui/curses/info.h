/**
 * Main header for the interface. Has all the structs and prototypes.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#ifndef _CURSES_INFO_H_
#define _CURSES_INFO_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "zen_ui.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

#ifdef DEBUG
#define NDEBUG
#endif /* DEBUG */

/* Number of pixels in a character cell. */
#define PIXELCOUNT 8

/**
 * Alignment of windows.
 * 
 * @member LEFT Aligns window to the left of screen.
 * @member CENTER Aligns window in the screens center.
 * @member RIGHT Aligns window in the right of screen.
 * @member TOP Aligns window in at the top of screen.
 * @member MIDDLE Aligns window in the screens center.
 * @member BOTTOM Aligns window on the bottom of screen.
 */
enum curses_window_align {
   LEFT, CENTER, RIGHT,
   TOP = 0, MIDDLE, BOTTOM
};

/**
 * Alignment of the interface.
 * 
 * @member horozontal Horozontal alignment.
 * @member vertical Vertical alignment.
 */
struct curses_align {
     enum curses_window_align horozontal;
     enum curses_window_align vertical;
};

/**
 * Basic unit of the interface.
 * 
 * @member name Name of the window.
 * @member window Drawing area.
 * @member height Height of window.
 * @member width Width of window.
 * @member top Top(Y) position of window.
 * @member left Left(X) position of window.
 */
struct curses_window {
   char *name;
   WINDOW *window;
   int height, width, top, left;
   struct curses_align align;
};

/**
 * Holds information about the whereabouts of a link.
 *
 * @member link The geometry of the link. 
 * @member part A pointer to the actual layout part which represent the link.
 * @member visual_part A pointer to the layout part which is displayed.
 * @member next A pointer to the next link in the list.
 * @member previous A pointer to the previous link in the list.
 */
struct curses_link {
   struct layout_rectangle link;
   struct layout_part *part;
   struct layout_part *visual_part;
   struct curses_link *next;
   struct curses_link *previous;
};

/**
 * Description of page.
 * 
 * @member links List of links for this page.
 * @member activelink Active link on the page.
 * @member next Next page.
 * @member previous Previous page.
 */
struct curses_page {
   struct curses_link *links;
   struct curses_link *activelink;
   struct layout_rectangle scroll;
   struct layout_rectangle geometry;
   struct layout_part *parts;
   struct curses_page *next;
   struct curses_page *previous;
};

/**
 * Describes a keybinding.
 * 
 * @member key Array of keys to test for.
 * @member index Number of keys in key.
 * @member name Name of key.
 * @member handler Function to call with that key.
 * @member next Next keybinding.
 * @member previous Previous keybinding.
 */
struct curses_key {
   int *key;
   int index;
   char *name;
   int (*handler)(void);
   struct curses_key *next;
   struct curses_key *previous;
};
   
/**
 * Description of the interface.
 * 
 * @member canvas Window for the canvas, where things are rendered.
 * @member titlebar This holds the title of the active page.
 * @member statusbar Bar that displays status messages.
 * @member activepage Description of current page.
 * @member pages List of pages visited.
 */
struct curses_interface {
   struct curses_window *canvas;
   struct curses_window *titlebar;
   struct curses_window *statusbar;
   struct curses_page *activepage;
   struct curses_page *pages;
   struct zen_ui_functions *functions;
   struct zen_ui_display *display;
   struct zen_ui_settings *settings;
   struct curses_key *keys;
};

/* This is in curses_init.c */
extern struct curses_interface curses_interface;

/* All that abstractation came at a cost in the number of functions */
extern int curses_close(void);
extern int curses_percent(int total, int percent);
extern int curses_pixels(int chars);
extern int curses_set_size(struct layout_part *partp);
extern int curses_interface_init(struct curses_interface *this,
			         struct zen_ui_functions *functions,
			         struct zen_ui_display *display,
			         struct zen_ui_settings *settings);
extern int curses_interface_open(struct curses_interface *this, char *url);
extern int curses_interface_close(struct curses_interface *this);
extern struct curses_key *curses_key_new(char *name, int key,
				    int (*handler)(struct curses_interface*),
					 struct zen_ui_functions *function);
extern struct curses_key *curses_key_linkin(struct curses_key *this,
		                            struct curses_key *newlink);
extern int curses_key_configlist(struct curses_key *this,
		  	         struct zen_ui_functions *functions);
extern int curses_key_pressed(struct curses_key *this,
		              struct curses_interface *interface, int key);
extern void curses_key_deleteall(struct curses_key *this);
extern int curses_open(char *url);
extern struct curses_page *curses_page_new(struct layout_part *parts);
extern struct curses_page *curses_page_linkin(struct curses_page *this,
 		                              struct curses_page *inductee);
extern void curses_page_nextlink(struct curses_page *this);
extern void curses_page_previouslink(struct curses_page *this);
extern int curses_page_scroll(struct curses_page *this,
		              int x_offset, int y_offset);
extern int curses_page_render(struct curses_page *this,
		              struct curses_window *window);
extern void curses_page_deleteall(struct curses_page *this);
extern int curses_lib_init(void);
extern void curses_lib_deinit(void);
extern int curses_window_new(char *name, int height, int width,
		             enum curses_window_align h,
		             enum curses_window_align v,
		             struct zen_ui_functions *function);
extern void curses_window_constrain(struct curses_window *first,
			            struct curses_window *second,
			            struct curses_window *third);
extern int curses_window_open(struct curses_window *this);
extern int curses_window_config(struct curses_window *this,
			        struct zen_ui_functions *function);
extern int curses_window_close(struct curses_window *this);
extern int curses_window_getkey(struct curses_window *this);
extern void curses_window_settext(struct curses_window *this, char *text);
extern int curses_window_gettext(struct curses_window *this,
			         char *dest, char *prompt, int length);
extern int curses_window_refresh(struct curses_window *this);

#endif /* _CURSES_INFO_H_ */
