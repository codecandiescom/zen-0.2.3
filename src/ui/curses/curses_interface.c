/**
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include "info.h"

/* ID of page being waited on. */
static int page_id;

/* URL storage, since this variable is wide used */
static char url[2000];

/* Status message. This one is well used too. */
static char status[2048];

/**
 * This updates the interface, makeing sure that things went right.
 * 
 * @return Returns 1 on failure and 0 on success.
 */
static int interface_update(struct curses_interface *this)
{
   if (curses_page_render(this->activepage, this->canvas)) return 1;
   if (curses_window_refresh(this->canvas)) return 1;
   
   return 0;
}

/**
 * Function called when pageup key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int pageup(struct curses_interface *this)
{
   if (curses_page_scroll(this->activepage, 0,
		          -curses_pixels(this->canvas->height)))
     return 1;
   
   if (interface_update(this)) return 1;
   
   return 0;
}

/**
 * Function called when pagedown key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int pagedown(struct curses_interface *this)
{
   if (curses_page_scroll(this->activepage, 0,
		          curses_pixels(this->canvas->height)))
     return 1;
   
   if (interface_update(this)) return 1;
   
   return 0;
}

/**
 * Function called when nextlink key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int nextlink(struct curses_interface *this)
{
   curses_page_nextlink(this->activepage);
   curses_window_text(this->activepage->activelink->data.link.href);
   if (interface_update(this)) return 1;
      
   return 0;
}

/**
 * Function called when previouskey key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int previouslink(struct curses_interface *this)
{
   curses_page_previouslink(this->activepage);
   curses_window_text(this->activepage->activelink->data.link.href);
   if (interface_update(this)) return 1;
   
   return 0;
}

/**
 * Function called when followlink key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int followlink(struct curses_interface *this)
{
   page_id = this->functions->request_page(
     this->activepage->activelink->data.link.href, NULL);
   
   sprintf(status, "Loading: %s...",
	   this->activepage->activelink->data.link.href);
   curses_window_text(this->statusbar);
   
   return 0;
}

/**
 * Function called when back key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int back(struct curses_interface *this)
{
   if (this->activepage->previous)
     this->activepage = this->activepage->previous;
   
   if (interface_update(this)) return 1;
   
   return 0;
}

/**
 * Function called when forward key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int forward(struct curses_interface *this)
{
   if (this->activepage->next)
     this->activepage = this->activepage->next;
   
   if (interface_update(this)) return 1;
   
   return 0;
}

/**
 * Function called when go key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int go(struct curses_interface *this)
{  
   if (curses_window_gettext(this->statusbar, url,
			     "Enter URL: ", 2000) < 0) return 1;
   
   sprintf(status, "Loading %s...", url);
   curses_window_settext(this->statubar, status);
   
   page_id = this->functions->request_page(url, NULL);
   
   return 0;
}

/**
 * Function called when quit key is hit.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
static int quit(struct curses_interface *this)
{
   /* Not the ideal solution, but it should work. */
   raise(SIGQUIT);
   return 0;
}

/**
 * Does all those things which must be done by init(). This is
 * really important for configurable things like windows and keystrokes.
 * It's called from init() and must come before we can do anything with
 * the interface.
 * 
 * @param this Interface to initilize.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
int curses_interface_init(struct curses_interface *this,
			  struct zen_ui_functions *functions,
			  struct zen_ui_display *display,
			  struct zen_ui_settings *settings)
{
   struct curses_key *binding, *ret;
   
   assert(this);
   
   /* Set the default keybindings. */
   binding = curses_key_new("pageup", KEY_PPAGE, pageup);
   if (!binding) {
     fprintf(stderr, "Error: Cannot set the default key bindings:\n");
     return 1;
   }
   
   /* The order of testing may be significant. This is clumsy, but it's a
    * first try.
    */
   ret = curses_key_linkin(binding, curses_key_new("quit", 'q', quit));
   ret = curses_key_linkin(binding, curses_key_new("go", 'g', go));
   ret = curses_key_linkin(binding, curses_key_new("forward", 'f', forward));
   ret = curses_key_linkin(binding, curses_key_new("pagedown", KEY_NPAGE, pagedown));
   ret = curses_key_linkin(binding, curses_key_new("pageup", KEY_PPAGE, pageup));
   ret = curses_key_linkin(binding, curses_key_new("nextlink", KEY_RIGHT, nextlink));
   ret = curses_key_linkin(binding, curses_key_new("previouslink", KEY_LEFT, previouslink));
   ret = curses_key_linkin(binding, curses_key_new("followlink", KEY_ENTER, followlink));
   ret = curses_key_linkin(binding, curses_key_new("back", KEY_BACKSPACE, back));
   
   if (ret) {
     fprintf(stderr, "Error: Cannot set default keybindings together:\n")
     return 1;
   }
   
   /* Set the windows and their default values. */
   this->canvas =
     curses_window_new("canvas", 80, 100, RIGHT, MIDDLE, functions);
   
   if (!this->canvas) {
     fprintf(stderr, "Error: Cannot create canvas:\n");
     return 1;
   }
   
   this->titlebar =
     curses_window_new("titlebar", 10, 100, RIGHT, TOP, functions);
   
   if (!this->titlebar) {
     fprintf(stderr, "Error: Cannot create titlebar:\n");
     return 1;
   }
   
   this->statusbar =
     curses_window_new("statusbar", 10, 100, RIGHT, BOTTOM, functions);
 
   if (!this->statusbar) {
     fprintf(stderr, "Error: Cannot create statusbar:\n");
     return 1;
   }
   
   this->keys = binding;
   this->functions = functions;
   this->display = display;
   this->settings = settings;
   this->activepage = this->pages = NULL;
   
   return 0;
}

/**
 * This actually opens and runs the interface.
 * 
 * @param this Interface to run.
 * @param url URL to open and display.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
int curses_interface_open(struct curses_interface *this, char *url)
{
   int key, error;
   struct layout_part *tmp_part = NULL;
   
   if (curses_lib_init()) {
     fprintf(stderr, "Error: Cannot initialize interface:\n");
     return 1;
   }
   
   /* Configure the windows. */
   error = curses_window_config(this->canvas);
   error = curses_window_config(this->titlebar);
   error = curses_window_config(this->statusbar);
   if (error) {
     fprintf(stderr, "Error: Failed to configure windows:\n");
     return 1;
   }
   
   /* Build window constraints. */
   curses_window_constrain(this->canvas,
			   this->titlebar,
			   this->statusbar);
   
   /* Open the windows. */
   error = curses_window_open(this->canvas);
   error = curses_window_open(this->titlebar);
   error = curses_window_open(this->statusbar);
   if (error) {
     fprintf(stderr, "Error: Failed to open windows:\n");
     return 1;
   }
  
   /* It'd be a pain to refresh these two every time I use them. */
   immedok(this->titlebar->window, TRUE);
   immedok(this->statusbar->window, TRUE);
   
   /* Ensure good keybindings. */
   error = curses_key_configlist(this->keys);
   if (error) {
     fprintf(stderr, "Error: Failed to configure keybindings:\n");
     return 1;
   }
   
   /* Also ensure display data is set... */
   this->display->width = curses_pixels(this->canvas->width);
   this->display->height = curses_pixels(this->canvas->height);
   
   /* ...and that we have an anchor page. */
   this->pages = curses_page_new(NULL);
   
   page_id = this->functions->request_page(url, NULL);
   sprintf(status, "Loading: %s...", url);
   curses_window_settext(this->statusbar, status);
   
   while (1) {
      if (this->functions->get_status(page_id, status, 2047) > 0)
        curses_window_settext(this->statusbar, status);
      
      error = this->functions->poll_page(page_id, &tmp_part);
      if (!error && tmp_part) {
	this->activepage =
	   curses_page_linkin(this->pages, curses_page_new(tmp_part));
	curses_window_text(this->statusbar, NULL);
        interface_update(this);
      }
      
      if ((key = curses_window_getkey(this->statusbar))) {
        error = curses_key_pressed(this->keys, interface, key);
	if (error)
	  fprintf(stderr, "Warning: There was an error with key %c:\n", key);
      }
   }
   
   return 0;
}

/**
 * Closes the interface, makeing sure the memory is free.
 * 
 * @param this Interface to deinitialize.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
int curses_interface_close(struct curses_interface *this)
{
   int ret;
   
   curses_lib_deinit();
   ret = curses_window_close(this->canvas);
   ret = curses_window_close(this->titlebar);
   ret = curses_window_close(this->statusbar);
   curses_key_deleteall(this->keys);
   
   /* In case of a really early exit, ensure there are pages to free. */
   if (this->pages)
     curses_page_deleteall(this->pages);
   
   if (ret != 0)
     fprintf(stderr, "Error: Could not close interface:\n");
   
   return ret;
}
