/**
 * Functions to retrieve font metrics from AFM files (Adobe Font
 * Metrics) and to hand out relevant values to the rest of the
 * program.
 *
 * @author Magnus Henoch <mange@freemail.hu>
 */

/*
 * Copyright (C) 2002, Magnus Henoch <mange@freemail.hu>
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
#include <string.h>
#include <stdlib.h>

#include "layout.h"
#include "info.h"

#include "psdump_parseAFM.h"

/* This is used when compiling with the libdmalloc debug library. */
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif /* HAVE_DMALLOC_H */

/* To avoid reading the AFM file each time the font is changed, let
 * alone each time a character is queried, FontInfo structs are saved
 * here. This is only a cache size limit, and will not prevent using
 * more fonts. Only eight entries should be needed, for
 * {Times,Courier} {regular,bold,italic,bold-italic}. Note that the
 * AFM file parser currently does not free allocated memory.
 */
#define MAX_FONTINFOS 10

/* The font name to be looked up is in fonts[x]->gfi->fontName
 * (e.g. "Times-Roman")
 */
static FontInfo *fonts[MAX_FONTINFOS];
static int fonts_in_cache = 0;

/**
 * Free memory allocated by psdump_parseFile.
 *
 * @param fi FontInfo struct to be deallocated
 */
static void free_fontinfo(FontInfo *fi) {
  int i, j;
  Ligature *l, *l2;

  if (fi == NULL)
    return;

  if (fi->gfi) {
    free(fi->gfi->afmVersion);
    free(fi->gfi->fontName);
    free(fi->gfi->fullName);
    free(fi->gfi->familyName);
    free(fi->gfi->weight);
    free(fi->gfi->version);
    free(fi->gfi->notice);
    free(fi->gfi->encodingScheme);
    free(fi->gfi);
  }
  free(fi->cwi);

  for (i = 0; i < fi->numOfChars; i++) {
    free(fi->cmi[i].name);
    l = fi->cmi[i].ligs;
    while (l) {
      free(l->succ);
      free(l->lig);
      l2 = l->next;
      free(l);
      l = l2;
    }
  }
  free(fi->cmi);

  free(fi->tkd);

  for (i = 0; i < fi->numOfPairs; i++) {
    free(fi->pkd[i].name1);
    free(fi->pkd[i].name2);
  }
  free(fi->pkd);

  for (i = 0; i < fi->numOfComps; i++) {
    free(fi->ccd[i].ccName);
    for (j = 0; j < fi->ccd[i].numOfPieces; j++) {
      free(fi->ccd[i].pieces[j].pccName);
    }
    free(fi->ccd[i].pieces);
  }
  free(fi->ccd);

  free(fi);
}

/**
 * Get FontInfo struct from the cache. If it is not
 * there, load it.
 *
 * @param fontname font name, e.g. "Times-Roman"
 *
 * @return pointer to FontInfo struct, or NULL on error
 */
static FontInfo *get_fontinfo(char *fontname) {
  int i, result;
  char *filename, *setbuf;
  FILE *afmfile;
  FontInfo *fontinfo;

  /* Is the font in the cache? */
  for (i = 0; i < fonts_in_cache; i++) {
    if (!strcmp(fontname, fonts[i]->gfi->fontName))
      return fonts[i];
  }

  /* If cache is full, remove last one. This is not
   * FIFO, but it is good enough.
   */
  if (fonts_in_cache >= MAX_FONTINFOS)
    fonts_in_cache = MAX_FONTINFOS - 1;

  /* AFM files are listed in the configuration, in
   * the form psdump_afm_Font-Weight = /path/to/something.afm
   */
  setbuf = malloc(strlen(fontname) + 12);
  strcpy(setbuf, "psdump_afm_");
  strcat(setbuf, fontname);
  if (psdump_ui->ui_functions->get_setting(setbuf, (void*)&filename) != SETTING_STRING)
    return NULL;
  afmfile = fopen(filename, "r");
  if (afmfile == NULL)
    return NULL;
  /* P_GW: get General info and character Widths */
  result = psdump_parseFile(afmfile, &fontinfo, P_GW);
  fclose(afmfile);
  if (result != ok) {
    free_fontinfo(fontinfo);
    return NULL;
  }

  fonts[fonts_in_cache] = fontinfo;
  fonts_in_cache++;
  return fontinfo;
}

/**
 * Return the width of a given string printed in a given font with
 * a given font size.
 *
 * @param string the string to be measured
 * @param fontname name of the font, like "Times-Roman"
 * @param fontsize font size in points
 *
 * @return width in points, or negative value on error
 */
double psdump_get_string_width(char *string, char *fontname, int fontsize) {
  FontInfo *fontinfo;
  double sum = 0.0, ratio;
  int i, c;

  fontinfo = get_fontinfo(fontname);
  if (fontinfo == NULL)
    return -1;

  /* What is the real algorithm? It seems I will have to RTFM. */
  ratio = (double)fontsize / /*(double)fontinfo->gfi->xHeight*/ 1000.0;
  for (i = 0; i < strlen(string); i++) {
    c = (int)(unsigned char)string[i];
    if (c < 256)
      sum += fontinfo->cwi[c] * ratio;
  }

  return sum;
}

/**
 * Choose a font according to information in layout_part, and
 * return in supplied buffer.
 *
 * @param partp Text layout part to be examined
 * @param font Buffer for font name. 30 bytes should be more than enough.
 *
 * @return Non-zero if successful, else zero (in particular,
 * @return if layout part was not text)
 */
int psdump_choose_font(struct layout_part *partp, char *font) {
  if (partp->type != LAYOUT_PART_TEXT)
    return 0;

  /* Select the best suited font for this mission.
   *
   * To use font X, where X is Helvetica or Courier, use
   * following styles:
   *
   * X (regular), X-Bold, X-Oblique (italic), X-BoldOblique
   *
   * If X is Times, use:
   *
   * X-Roman, X-Bold, X-Italic, X-BoldItalic
   *
   * Right now, only Times and Courier are used. This might
   * be made configurable in the future.
   */
  if (partp->data.text.style.monospaced) {
    strcpy(font, "Courier");
    if (partp->data.text.style.bold) {
      if (partp->data.text.style.italic) {
	strcat(font, "-BoldOblique");
      } else {
	strcat(font, "-Bold");
      }
    } else {
      if (partp->data.text.style.italic) {
	strcat(font, "-Oblique");
      } else {
	/* no style - add nothing */
      }
    }
  } else {
    strcpy(font, "Times");
    if (partp->data.text.style.bold) {
      if (partp->data.text.style.italic) {
	strcat(font, "-BoldItalic");
      } else {
	strcat(font, "-Bold");
      }
    } else {
      if (partp->data.text.style.italic) {
	strcat(font, "-Italic");
      } else {
	strcat(font, "-Roman");
      }
    }
  }
  return 1;
}

