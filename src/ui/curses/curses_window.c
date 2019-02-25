/**
 * This is the lowest code to the curses library, handles more complicated 
 * things -- to keep everything organized.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include <ctype.h>
#include <string.h>

#include "info.h"

/**
 * Initializes the curses library.
 * 
 * @returns 0 on success and 1 on failure.
 */
int curses_lib_init(void)
{
   if (!initscr()) {
     fprintf(stderr, "Error: Could not initalize curses library:\n");
     return 1;
   }
   
   cbreak();
   noecho();
   nonl();
   intrflush(stdscr, FALSE);
   keypad(stdscr, TRUE);
   
   return 0;
}

/**
 * Uninitializes the curses library.
 */
void curses_lib_deinit(void)
{
   endwin();
}

/**
 * This notifies Zen's setting code of the window options.
 * 
 * @param name Name of the window.
 * @param height Default height.
 * @param width Default width.
 * @param h Horozontal align.
 * @param v Vertical align.
 * @param function Functions for notifing Zen.
 * 
 * @return Returns 0 on success and non-zero on failure.
 */
static int notify_setting(char *name, int height, int width,
			  enum curses_window_align h,
			  enum curses_window_align v,
			  struct zen_ui_functions *function)
{
   int ret;
   char optname[strlen(name) + 1 + 7 + 12];
   
   assert(name && window && height && width && h && v && function);
   
   sprintf(optname, "%s%s%s", name, "_window", "_height");
   ret = function->set_setting(optname, height, SETTING_NUMBER);
   
   sprintf(optname, "%s%s%s", name, "_window", "_width");
   ret = function->set_setting(optname, width, SETTING_NUMBER);
   
   sprintf(optname, "%s%s%s", name, "_window", "_horozontal");
   ret = function->set_setting(optname, h, SETTING_NUMBER);
   
   sprintf(optname, "%s%s%s", name, "_window", "_vertical");
   ret = function->set_setting(optname, v, SETTING_NUMBER);
   
   return ret;
}

/**
 * Makes new window description and sets defaults for it.
 * 
 * @param y Y position of window.
 * @param x X position of window.
 * @param height Height of window.
 * @param width Width of window.
 * @param h Horozontal alignment.
 * @param v Vertical alignment.
 * @param function Ugly thing needed in order to use settings.
 * 
 * @returns 1 of failure, a 0 on success.
 */
int curses_window_new(char *name, int height, int width,
		      enum curses_window_align h,
	              enum curses_window_align v,
	              struct zen_ui_functions *function)
{
   struct curses_window *this =
     (struct curses_window *) malloc(sizeof(struct curses_window));
   
   assert(name && height && width && h && v && function);
   
   if (!this) {
     fprintf(stderr, "Error: Cannot allocate memory for new window:\n");
     return 1;
   }
   
   this->name = malloc(len);
   if (!this->name) {
     fprintf(stderr, "Error: Cannot allocate space for window's name:\n");
     free(this);
     return 1;
   }
   
   /* These manual settings will likely disappear when the config
    * code is proven to work.
    */
   this->align.vertical = v;
   this->align.horozontal = h;
   this->width = curses_percent(COLS, width);
   this->height = curses_percent(LINES, height);
     
   if (notify_setting(name, height, width, h, v, function)) {
     fprintf(stderr, "Error: Cannot set default "
	     "settings for window %s:\n", name);
     free(this->name);
     free(this);
     return 1;
   }
   
   return 0;
}

/**
 * Sets the right constraint for window (just to reduce redundant code).
 * 
 * @param window Window to set.
 */
static void set_left(struct curses_window *this)
{
   switch (this->align.horozontal) {
    case LEFT: this->left = 0; break;
    case CENTER: this->left = (COLS - this->width) / 2; break;
    case RIGHT: this->left = COLS - this->width;
   }
}


/**
 * This builds constraints for the windows. Depending on the configuration
 * of the windows, each will be assigned a top and left constraint where their
 * top left corner will be placed. These constraints are assigned based on
 * their width, height, and alignments. We do it here because it should be
 * seperate from everything else.
 * 
 * NOTE: This is not as flexible as it should be (it's crude), and only
 *       deals with three windows at the moment. Hopefully I can come up with
 *       a more flexible method later on.
 * 
 * @param first First of the windows.
 * @param second Second.
 * @param third Third.
 */
void curses_window_constrain(struct curses_window *first,
			     struct curses_window *second,
			     struct curses_window *third)
{
   struct curses_window *top, *middle, *bottom;
   
   if (first->align.vertical == TOP) top = first;
   else if (second->align.vertical == TOP) top = second;
   else top = third;
   
   if (first->align.vertical == MIDDLE) middle = first;
   else if (second->align.vertical == MIDDLE) middle = second;
   else middle = third;
   
   if (first->align.vertical == BOTTOM) bottom = first;
   else if (second->align.vertical == BOTTOM) bottom = second;
   else bottom = third;
   
   top->top = 0;
   set_left(top);
   
   middle->top = top->height;
   set_left(middle);
   
   bottom->top = middle->top + middle->height;
   set_left(bottom);
}

/**
 * Opens a configured window, so it can actually be used.
 * 
 * @param window Window to open.
 *
 * @return Returns 0 on success and 1 on failure.
 */
int curses_window_open(struct curses_window *this)
{
   assert(this);
   
   this->window = newwin(this->height, this->width, 
			 this->top, this->left);
   
   if (!this->window) {
     fprintf(stderr, "Error: Cannot open a new window:\n");
     return 1;
   }
   
   /* Default configs for WINDOWs */
   keypad(this->window, TRUE);
   
   return 0;
}

/**
 * Configures the window.
 * 
 * @param window Window to initialize.
 * @param function Functions to call zens config code.
 *
 * @return Returns 0 on success and 1 on failure.
 */
int curses_window_config(struct curses_window *this,
			 struct zen_ui_functions *function)
{
   assert(this && name && function);
   return 0;
}

/**
 * Deletes memory for a window.
 * 
 * @param window Window to delete.
 */
static void window_delete(struct curses_window *this)
{
   assert(this);
   
   free(this->name);
   free(this);
}

/**
 * Closes a window.
 * 
 * @param window Window to close.
 * @returns Returns 1 on failure, 0 on success.
 */
int curses_window_close(struct curses_window *this)
{
   assert(this);
   
   if (delwin(this->window) == ERR) {
     fprintf(stderr, "Error: Could not close window:\n");
     return 1;
   }

   window_delete(this);
   
   return 0;
}

/**
 * Gets a key from window.
 * 
 * @param this Window to get key from.
 * 
 * @return Returns the key pressed.
 */
int curses_window_getkey(struct curses_window *this)
{
   int key;
   
   if ((key = wgetch(this->window)) == ERR)
     return 0;
   
   return key;
}

/**
 * Sets the text on a window. (Assumes immedok is TRUE).
 * 
 * @param this Window to set the text on.
 * @param text Text to put on the window.
 */
void curses_window_settext(struct curses_widow *this, char *text)
{
   assert(this);
   werase(this->window);
   
   if (text)
     waddstr(this->window, text);
}

/**
 * Prompts the window for text input. Answer is returned to the caller.
 * (Assumes that nodelay is FALSE, as is default for curses). This is really
 * simple right now.
 * 
 * @param this Window to get text from.
 * @param dest Destination of the text.
 * @param prompt Prompt for the text.
 * @param length Max lenght that the input can be.
 * 
 * @return Returns number of characters in dest.
 */
int curses_window_gettext(struct curses_window *this,
			  char *dest, char *prompt, int length)
{
  int i, key;
   
  assert(this && dest && length);
  
  dest[0] = '\0';
  waddstr(this->window, prompt);
  wmove(this->window, 1, strlen(prompt) + 1);
   
  for (i = 0; i < length && (key = wgetch(this->window)) != '\n'; i++)
     switch (key) {
      case KEY_LEFT:
      case KEY_RIGHT:
      case KEY_HOME:
      case KEY_END:
	/* We don't handle them right now. */
	break;
	
      default:
	if (isprint(key) {
	  dest[i] = key;
	  dest[i+1] = '\0';
	}
     }
   
   return i;
}

/**
 * Updates window. Just here in case something has to be done explicitly
 * in the future.
 * 
 * @param window Window to update.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
int curses_window_refresh(struct curses_window *this)
{
  assert(this);
   
  if (wrefresh(this->window) == ERR)
    return 1;
   
  return 0;
}
