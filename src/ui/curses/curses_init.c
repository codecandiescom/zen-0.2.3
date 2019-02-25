/**
 * Initialize the interface and prepare it for the task to come.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include "zen_ui.h"
#include "version.h"
#include "info.h"

/* This is global for the sake of curses_close() and curses_open().
 * All the other references to it are pointers.
 */
struct curses_interface curses_interface;

/**
 * @param argc The number of arguments given on the command line.
 * @param argv An array of arguments given on the command line. 
 * @param ui A zen_ui struct that contains information about the
 * @param ui curses user interface. 
 *
 * @return non-zero value if an error occurred.
 */
int init(int argc, char *argv[], struct zen_ui *ui)
{
  static struct zen_ui_operations curses_operations = {
    curses_open,
    curses_close,
    curses_set_size,
    NULL
  };
   
  ui->ui_type = ZEN_UI_GRAPHICAL;
  ui->ui_operations = &curses_operations;
  ui->ui_name = UI_NAME;
  ui->ui_version_text = UI_VERSION_TEXT;
  ui->ui_version_number = UI_VERSION_NUMBER;

  ui->ui_support.interaction = 1;
  ui->ui_support.freemove = 1;
  ui->ui_support.scrollable_y = 1;

  ui->ui_settings.min_fontsize = 8;
  ui->ui_settings.max_fontsize = 8;
  ui->ui_settings.default_fontsize = 8;
   
  if (curses_interface_init(&curses_interface, ui->ui_functions,
		            &ui->ui_display, &ui->ui_settings)) {
    fprintf(stderr, "Error: Initialization failed:\n");
    return 1;
  }
   
  return 0;
}
