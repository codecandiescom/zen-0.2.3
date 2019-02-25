/**
 * Helper functions.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include "info.h"

/**
 * Translates a percent into usable values.
 * 
 * @param total Total to take percent from.
 * @param percent Percent to return of total.
 * 
 * @return Returns percent of total (pretty close anyway).
 */
int curses_percent(int total, int percent)
{
   assert(total && percent);
   return div(total, percent).quot;
}

/**
 * Converts chars, which are the unit of measure ment in curses, into pixels;
 * Zens measurement unit.
 * 
 * @param chars Curses style length.
 */
int curses_pixels(int chars)
{
   return chars * PIXELCOUNT;
}

/**
 * Set the width and height of a part, which only the user interface
 * can know the width and height of. This includes text strings and
 * various widgets in forms.
 *
 * @param partp A pointer to the layout part to set the size of.
 *
 * @return non-zero value if an error occurred.
 */
int curses_set_size(struct layout_part *partp)
{
  switch(partp->type) {
  case LAYOUT_PART_TEXT:
    partp->geometry.width = curses_pixels(strlen(partp->data.text.text));
    partp->geometry.height = curses_pixels(1);
    break;

  case LAYOUT_PART_FORM_SUBMIT:
    partp->geometry.width =
       curses_pixels(strlen(partp->data.form_submit.value));
    partp->geometry.height = curses_pixels(1);
    break;

  default:
  }

  return 0;
}
