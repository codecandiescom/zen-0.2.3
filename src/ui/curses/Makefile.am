AM_CPPFLAGS = -I.. -I../.. -I../../.. -I../../layouter

# Prevent any other library to link into this one. 
# But we want to link the debug library into this. Maybe not.
LIBS = # @DEBUG_LIBS@

uidir = @UIDIR@

ui_LTLIBRARIES = libzen_ui_curses.la
libzen_ui_curses_la_SOURCES = curses_init.c curses_open.c curses_close.c \
			      curses_interface.c curses_page.c curses_key.c \
			      curses_window.c curses_helpers.c \
			      version.h info.h

libzen_ui_curses_la_LDFLAGS = -version-info 0:1:0

libzen_ui_curses_la_LIBADD = @CURSES_LIBS@
