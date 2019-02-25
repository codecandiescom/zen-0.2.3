AM_CPPFLAGS = -I.. -I../.. -I../../.. -I../../layouter

# Prevent any other library to link into this one. 
LIBS =

uidir = @UIDIR@

ui_LTLIBRARIES = libzen_ui_dump.la
libzen_ui_dump_la_SOURCES = dump_render.c dump_helpers.c \
			    dump_init.c dump_open.c dump_close.c \
			    dump_text.c dump_image.c dump_line.c dump_title.c \
			    version.h info.h

libzen_ui_dump_la_LDFLAGS = -version-info 0:1:0

