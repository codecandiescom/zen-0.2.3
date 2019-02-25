/**
 * Functions which handle the keybindings for the interface.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include <string.h>

#include "info.h"

/**
 * This notifies the Zen setting code of the key settings. For the moment
 * we treat keys as single characters, and do not handle key specified as
 * `KEY_UP' or anything. For simplicity.
 * 
 * @param name Name of key.
 * @param key Value.
 * @param function Functions.
 * 
 * @return Returns 1 on failure and 0 on success.
 */
int notify_setting(char *name, int key, struct zen_ui_functions *function)
{
   char setting[strlen(name)+5];
   sprintf(setting, "%s%s", name, "_key");
   
   function->set_setting(setting, (void*)key, SETTING_NUMBER);
   return 0;
}

/**
 * Creates a new keybinding.
 * 
 * @param name Name of the key (used in the configuration).
 * @param key Default key to associate with this handler.
 * @param handler Function to call when this key is pressed.
 * 
 * @return Returns NULL on failure and a key on success.
 */
struct curses_key *curses_key_new(char *name, int key,
				  int (*handler)(struct curses_interface*),
				  struct zen_ui_functions *function)
{
   int len = strlen(name) + 1;
   struct curses_key *this =
     (struct curses_key *) malloc(sizeof(struct curses_key));
   
   assert(name && handler && function);
   
   if (!this) {
     fprintf(stderr, "Error: Cannot allocate memory for new key:\n");
     return NULL;
   }
   
   if (notify_setting(name, key, function)) {
      fprintf(stderr, "Error: Cannot notify Zen of keybinding:");
      return NULL;
   }
   
   this->name = malloc(len);
   if (!this->name) {
     fprintf(stderr, "Error: Cannot allocate memory for "
	             "key name in new key:\n");
     free(this);
     return NULL;
   }
   
   strcpy(this->name, name);
   
   this->keys = (int*) calloc(1, sizeof(int));
   if (!this->keys) {
     fprintf(stderr, "Error: Cannot allocate memory for keys"
	             " in new key:\n");
     free(this->name);
     free(this);
     return NULL;
   }
   
   this->index = 1;
   this->keys[0] = key;
   this->handler = handler;
   this->next = this->previous = NULL;
   
   return this;
}

/**
 * Links a keybinding into a list of keybindings.
 * 
 * @param this List of keybindings.
 * @param newlink New keybinding to link in.
 */
struct curse_key *curses_key_linkin(struct curses_key *this,
		                    struct curses_key *newlink)
{
   assert(this);
   
   if (!newlink) {
      fprintf(stderr, "Error: No key to linkin:\n");
      return NULL;
   }
   
   for (; this->next; this = this->next)
     /* Do nothing */;
   
   newlink->next = NULL;
   newlink->previous = this;
   this->next = newlink;
   
   return newlink;
}

/**
 * Configures a key.
 * 
 * @param this The key to configure.
 * @param function Functions to configure with.
 * 
 * @return 0 on success and 1 on failure.
 */
static int key_config(struct curses_key *this,
	       struct zen_ui_functions *function)
{
   assert(this && function);
   return 0;
}

/**
 * Makes it easier to configure a list of keys.
 * 
 * @param this First in the list to configure.
 * 
 * @return Returns 0 on success and 1 on failure.
 */
int curses_key_configlist(struct curses_key *this,
		  	  struct zen_ui_functions *functions)
{
   for (; this->next; this = this->next)
     if (key_config(this)) return 1;
   
   if (key_config(this)) return 1;
   
   return 0;
}

/**
 * Find the key which was pressed and call the appropriate function.
 * 
 * @param this First key to check against.
 * @param key Value to test for.
 * 
 * @return Returns 0 if key is identified and executed, 1 otherwise.
 */
int curses_key_pressed(struct curses_key *this,
		       struct curses_interface *interface, int key)
{
   int i;
   
   assert(this && key);
   
   for (; this; this->next)
     for (i = 0; i < this->index; i++)
       if (this->keys[i] == key)
         return this->handler(interface);
   
   /* No match for the key, but still a success. */
   return 0;
}

/**
 * Deletes a key.
 * 
 * @param this Key to delete.
 */
static void key_delete(struct curses_key *this)
{
   assert(this);
   
   free(this->name);
   free(this->keys);
   free(this);
}

/**
 * Deletes all the keys.
 * 
 * @param this List to delete.
 */
void curses_key_deleteall(struct curses_key *this)
{
   assert(this);
   
   for (; this->previous; this = this->previous);
   for (this = this->next; this && this->next; this = this->next)
     key_delete(this->previous);
   
   key_delete(this);
}
