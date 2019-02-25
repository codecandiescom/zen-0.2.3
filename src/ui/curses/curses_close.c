/**
 * The close function. Probably could do with out this and call the interface
 * close in the first place -- but I stick to this in case I support multiple
 * interfaces.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include "info.h"

/**
 * Closes the interface.
 * 
 * @return Returns 1 on failure, 0 on success.
 */
int curses_close(void)
{
   return curses_interface_close(&curses_interface);
}
