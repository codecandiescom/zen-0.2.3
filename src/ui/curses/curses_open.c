/**
 * Opens and readies the interface.
 * 
 * @author Michael Pollard <smile3re@yahoo.com>
 */

#include <signal.h>

#include "info.h"

/* The signal information. */
static struct sigaction oldsigs[16];

/**
 * This is the fatal error signal handler. This will guide you from the
 * light, and into the darkness. It will also close the framebuffer and
 * exit abruptly in a very unclean way. I hope to be able to change this
 * behaviour some time in the future.
 *
 * @param sig The signal number which caused the signal handler to be called.
 */
static void signal_handler(int sig)
{
  curses_close();
}

/**
 * This sets up the interface to catch a number of signals that might occur
 * and disturb the task we are here to achieve. This is only for safety, in
 * order to be able to close the framebuffer before exiting the program, if
 * something would go too wrong.
 */
static void setup_signals(void)
{
  sigset_t mask;
  struct sigaction sigs;

  sigemptyset(&mask);

  sigs.sa_handler = signal_handler;
  sigs.sa_mask = mask;
  sigs.sa_flags = SA_ONESHOT;
  sigs.sa_restorer = NULL;

  sigaction(SIGHUP, &sigs, &oldsigs[0]);
  sigaction(SIGINT, &sigs, &oldsigs[1]);
  sigaction(SIGQUIT, &sigs, &oldsigs[2]);
  sigaction(SIGILL, &sigs, &oldsigs[3]);
  sigaction(SIGTRAP, &sigs, &oldsigs[4]);
  sigaction(SIGBUS, &sigs, &oldsigs[5]);
  sigaction(SIGFPE, &sigs, &oldsigs[6]);
  sigaction(SIGSEGV, &sigs, &oldsigs[7]);
  sigaction(SIGTERM, &sigs, &oldsigs[8]);
}

/**
 * Restore the signals set up by setup_signals().
 */
static void restore_signals(void)
{
  sigaction(SIGHUP, &oldsigs[0], NULL);
  sigaction(SIGINT, &oldsigs[1], NULL);
  sigaction(SIGQUIT, &oldsigs[2], NULL);
  sigaction(SIGILL, &oldsigs[3], NULL);
  sigaction(SIGTRAP, &oldsigs[4], NULL);
  sigaction(SIGBUS, &oldsigs[5], NULL);
  sigaction(SIGFPE, &oldsigs[6], NULL);
  sigaction(SIGSEGV, &oldsigs[7], NULL);
  sigaction(SIGTERM, &oldsigs[8], NULL);
}

/**
 * Open interface.
 * 
 * @param url The URL given on the command line for the program.
 *
 * @return non-zero value if an error occurred.
 */
int curses_open(char *url)
{
  int ret;

  setup_signals();
   
  /* Only display's width and height will be really important (although the
   * endian is set anyway). They can be set up only when the interface code
   * gets the settings and opens the windows... which will happen before we
   * retreive any pages, meaning things will be ok.
   */
  curses_interface.display->endian = ZEN_UI_DISPLAY_NATIVE_ENDIAN;

  ret = curses_interface_open(&curses_interface, url);
  restore_signals();

  return ret;
}
