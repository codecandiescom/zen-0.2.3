/**
 * Open function for the oFBis user interface.
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
#include <signal.h>
#include <ofbis.h>

#include "info.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

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
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;

  fprintf(stderr,"oFBis interface: signal %d caught and exits program!\n", 
	  sig);
  FBclose(info->ofbis_fb);
  exit(1);
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
 * Open the framebuffer with the settings of the current virtual console.
 * This might change later, so that the interface will be able to set
 * a different resolution. 
 *
 * Comments: We currently open a new virtual console for the interface,
 *           because it is easier to use while developing. Later, this
 *           might change to using the same console, or perhaps let the
 *           user decide.
 *
 * @param url The URL given on the command line for the program.
 *
 * @return non-zero value if an error occurred.
 */
int ofbis_open(char *url)
{
  struct ofbis_information *info = 
    (struct ofbis_information *)ofbis_ui->ui_specific;
  FB *fb;
  int nr_of_colours, i, r, g, b, ret;
  int *red, *green, *blue;

  fb = FBopen(NULL, FB_KEEP_CURRENT_VC);
  if(fb == NULL)
    return 1;

  info->ofbis_fb = fb;

  setup_signals();

  /* Now we can set the correct display values of the interface. */
  ofbis_ui->ui_display.width = fb->vinf.xres;
  ofbis_ui->ui_display.height = fb->vinf.yres;
  ofbis_ui->ui_display.bit_depth = fb->vinf.bits_per_pixel;
  ofbis_ui->ui_display.red_length = fb->vinf.red.length;
  ofbis_ui->ui_display.green_length = fb->vinf.green.length;
  ofbis_ui->ui_display.blue_length = fb->vinf.blue.length;
  ofbis_ui->ui_display.red_offset = fb->vinf.red.offset;
  ofbis_ui->ui_display.green_offset = fb->vinf.green.offset;
  ofbis_ui->ui_display.blue_offset = fb->vinf.blue.offset;
  ofbis_ui->ui_display.endian = ZEN_UI_DISPLAY_NATIVE_ENDIAN;

  /* Setup the current colourmap here. */
  if(fb->finf.visual == FB_VISUAL_PSEUDOCOLOR) {
    nr_of_colours = 1 << ofbis_ui->ui_display.bit_depth;
    ofbis_ui->ui_display.colourmap = 
      (unsigned char *)malloc(3 * nr_of_colours);
    if(ofbis_ui->ui_display.colourmap == NULL) {
      FBclose(fb);
      return 1;
    }

    /* Set the defaulted palette. */
    red = (int *)malloc(sizeof(int) * 256);
    if(red == NULL) {
      free(ofbis_ui->ui_display.colourmap);
      FBclose(fb);
      return 1;
    }
    green = (int *)malloc(sizeof(int) * 256);
    if(green == NULL) {
      free(red);
      free(ofbis_ui->ui_display.colourmap);
      FBclose(fb);
      return 1;
    }
    blue = (int *)malloc(sizeof(int) * 256);
    if(blue == NULL) {
      free(green);
      free(red);
      free(ofbis_ui->ui_display.colourmap);
      FBclose(fb);
      return 1;
    }

    i = 0;
    for(r = 0 ; r < 65536 ; r += 65535 / 5) {
      for(g = 0 ; g < 65536 ; g += 65535 / 5) {
	for(b = 0 ; b < 65536 ; b += 65535 / 5) {
	  /* For safety. */
	  if(i >= 216)
	    break;
	  red[i]   = ofbis_ui->ui_display.colourmap[3 * i + 0] = r >> 8;
	  green[i] = ofbis_ui->ui_display.colourmap[3 * i + 1] = g >> 8;
	  blue[i]  = ofbis_ui->ui_display.colourmap[3 * i + 2] = b >> 8;
	  i++;
	}
      }
    }
    for(r = 65535 / 15 ; r < 65536 ; r += 65535 / 15) {
      if((r % (65535 / 5)) != 0) {
	red[i]   = ofbis_ui->ui_display.colourmap[3 * i + 0] = r >> 8;
	green[i] = ofbis_ui->ui_display.colourmap[3 * i + 1] = 0;
	blue[i]  = ofbis_ui->ui_display.colourmap[3 * i + 2] = 0;
	i++;
      }
    }
    for(g = 65535 / 15 ; g < 65536 ; g += 65535 / 15) {
      if((g % (65535 / 5)) != 0) {
	red[i]   = ofbis_ui->ui_display.colourmap[3 * i + 0] = 0;
	green[i] = ofbis_ui->ui_display.colourmap[3 * i + 1] = g >> 8;
	blue[i]  = ofbis_ui->ui_display.colourmap[3 * i + 2] = 0;
	i++;
      }
    }
    for(b = 65535 / 15 ; b < 65536 ; b += 65535 / 15) {
      if((b % (65535 / 5)) != 0) {
	red[i]   = ofbis_ui->ui_display.colourmap[3 * i + 0] = 0;
	green[i] = ofbis_ui->ui_display.colourmap[3 * i + 1] = 0;
	blue[i]  = ofbis_ui->ui_display.colourmap[3 * i + 2] = b >> 8;
	i++;
      }
    }
    for(g = 65535 / 15 ; g < 65536 ; g += 65535 / 15) {
      if((g % (65535 / 5)) != 0) {
	red[i]   = ofbis_ui->ui_display.colourmap[3 * i + 0] = g >> 8;
	green[i] = ofbis_ui->ui_display.colourmap[3 * i + 1] = g >> 8;
	blue[i]  = ofbis_ui->ui_display.colourmap[3 * i + 2] = g >> 8;
	i++;
      }
    }

    if(i != 256)
      fprintf(stderr, 
	      "Warning! Palette index went wrong and ended up at %d!\n", 
	      i);

    ofbis_set_palette(red, green, blue);

    free(red);
    free(green);
    free(blue);
  }

  /* Enter the main event loop and pass on the given URL. */
  ret = ofbis_main(url);

  restore_signals();

  return ret;
}
