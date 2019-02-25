SUBDIRS = @AVAILABLE_UI@ progress_logo

# This is needed to ensure these are added when creating the distribution.
DIST_SUBDIRS = dump ofbis gtk curses psdump progress_logo

AM_CPPFLAGS = -I.. -I../.. -I../layouter

noinst_LIBRARIES = libui.a

libui_a_SOURCES = control.c functions.c \
		  functions.h ui.h zen_ui.h

