/**
 * Functions to initialize and exitialize the user interface we 
 * want to use. 
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
#include <string.h>
#include <dlfcn.h>

#include "paths.h"
#include "settings.h"
#include "zen_ui.h"
#include "layout.h"
#include "functions.h"
#include "threads.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/* The information about the user interface. */
struct zen_ui user_interface;

/* The handle for the dynamic library for the user interface. */
static void *dl_handle;

/**
 * This will find the correct user interface for us to use. It then 
 * opens the shared library that corresponds to that user interface
 * and the current version of Zen. When that is opened, it looks for
 * the init routine in that library, which just happens to be the
 * init routine for the user interface we are about to use.
 *
 * The decision for which interface to be used, has been moved from
 * this function, to the settings handler. This simply reads the 
 * previously made choice. Even though it should never ever happen, as
 * a silly security check, if the settings should be unavailable, we
 * default to the dump user interface.
 *
 * @return A pointer to the user interface init routine, or NULL if
 * @return it could not be found.
 */
static zen_ui_init_t *ui_find_user_interface(void)
{
  char *ui_lib_name;
  zen_ui_init_t *init_routine;
  char *error, *interface;
  int size;
  void *value;

  size = strlen(UIDIR) + 32;
  settings_get("interface", &value);
  interface = (char *)value;
  if(interface == NULL)
    interface = "dump";
  size += strlen(interface);

  ui_lib_name = (char *)malloc(size);
  if(ui_lib_name == NULL)
    return NULL;

  sprintf(ui_lib_name, "%s/libzen_ui_%s.so", UIDIR, interface);
  dl_handle = dlopen(ui_lib_name, RTLD_NOW);

  if(dl_handle == NULL) {
    fprintf(stderr, "Could not open the user interface library %s\n", 
	    ui_lib_name);
    fprintf(stderr, "Error: %s\n", dlerror());
    free(ui_lib_name);
    return NULL;
  }

  free(ui_lib_name);

  init_routine = dlsym(dl_handle, "init");

  error = dlerror();
  if(error != NULL) {
    fprintf(stderr, "%s\n", error);
    dlclose(dl_handle);
    return NULL;
  }

  return init_routine;
}

/**
 * Performs initialization of the user interface. This includes finding
 * out which user interface the user wants to use, load the appropriate
 * shared library and call its initialization routine. 
 *
 * @param argc The number of arguments given on the command line.
 * @param argv An array of arguments given on the command line. 
 *
 * @return non-zero value if an error occurred.
 */
int ui_init(int argc, char *argv[])
{
  zen_ui_init_t *init_routine;
  int ret;

  /* First of all we want to know if there is a user interface available
   * for us, and if it is willing to share its precious time on our
   * feeble wishes. Let us all hope so.
   */
  init_routine = ui_find_user_interface();
  if(init_routine == NULL)
    return 1;

  /* Set up default values. */
  user_interface.ui_type = ZEN_UI_UNSPECIFIED;
  user_interface.ui_operations = NULL;
  user_interface.ui_name = NULL;
  user_interface.ui_version_text = NULL;
  user_interface.ui_version_number = 0;
  user_interface.ui_support.table = 0;
  user_interface.ui_support.frame = 0;
  user_interface.ui_support.image = 0;
  user_interface.ui_support.font = 0;
  user_interface.ui_support.interaction = 0;
  user_interface.ui_support.freemove = 0;
  user_interface.ui_support.scrollable_x = 0;
  user_interface.ui_support.scrollable_y = 0;
  user_interface.ui_settings.min_fontsize = 8;
  user_interface.ui_settings.max_fontsize = 8;
  user_interface.ui_settings.default_fontsize = 8;
  user_interface.ui_display.width = -1;
  user_interface.ui_display.height = -1;
  user_interface.ui_display.red_length = 0;  
  user_interface.ui_display.green_length = 0;  
  user_interface.ui_display.blue_length = 0;  
  user_interface.ui_display.red_offset = 0;  
  user_interface.ui_display.green_offset = 0;  
  user_interface.ui_display.blue_offset = 0;  
  user_interface.ui_display.colourmap = NULL;
  user_interface.ui_specific = NULL;

  /* We need to initialize these helper functions before we have initialized
   * the user interface, so that it can set default settings values.
   */
  user_interface.ui_functions = ui_functions_init();

  /* Call the initialization routine for the user interface. */
  ret = init_routine(argc, argv, &user_interface);
  if(ret)
    return ret;

  /* If the user interface failed to set the ui_operations, 
   * we need to bail out quickly.
   */
  if(user_interface.ui_operations == NULL)
    return 1;

  return 0;
}

/**
 * The thread function which will open the user interface. This is only
 * used when threads are used.
 *
 * @param url NULL or a pointer to the URL given at the command line.
 *
 * @return The return code of the user interface.
 */
static void *ui_open_thread(void *url)
{
  long ret;

  /* This is here, because we want to make really, really sure,
   * that the thread is killed, and not just asked nicely to 
   * kind of, maybe exit itself, when it is suitable for it. 
   * We want it dead, when we tell it to die!
   */
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  ret = user_interface.ui_operations->open((char *)url);

  return (void *)ret;
}

/**
 * Open the initiated user interface. 
 *
 * @return non-zero value if an error occurred.
 */
int ui_open(char *url)
{
  int ret;

  if(user_interface.ui_support.interaction)
    ret = thread_start(THREAD_TYPE_INTERFACE, ui_open_thread, (void *)url);
  else
    ret = user_interface.ui_operations->open(url);

  return ret;
}

/**
 * Closes and cleans up after the user interface mess. 
 *
 * @return non-zero value if an error occurred.
 */
int ui_exit(void)
{
  int ret;

  if(user_interface.ui_support.interaction) {
    thread_wait(THREAD_TYPE_INTERFACE);
    thread_kill_all();
  }

  /* Close the user interface. */
  ret = user_interface.ui_operations->close();

#if 0
  /* Close the dynamic library containing the user interface we just dumped. */
  if(dl_handle)
    dlclose(dl_handle);
#endif

  return ret;
}

/**
 * Calls the user interface function for setting the palette, if there
 * is one provided.
 *
 * @param red An array of red elements of the palette.
 * @param green An array of green elements of the palette.
 * @param blue An array of blue elements of the palette.
 *
 * @return non-zero value if an error occurred.
 */
int ui_set_palette(int *red, int *green, int *blue)
{
  if(user_interface.ui_operations->set_palette != NULL)
    user_interface.ui_operations->set_palette(red, green, blue);  

  return 0;
}
