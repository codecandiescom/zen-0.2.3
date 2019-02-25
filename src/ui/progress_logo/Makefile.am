AM_CPPFLAGS =

# Prevent any other library to link into this one. 
LIBS = 

uidir = @UIDIR@

# This builds the shared library for the Zen progress logo.
ui_LTLIBRARIES = libzen_progress_zen.la
libzen_progress_zen_la_SOURCES = progress_zen.c 
libzen_progress_zen_la_LDFLAGS = -version-info 1:0:1

noinst_HEADERS = progress_logo.h
EXTRA_DIST = *.xpm
