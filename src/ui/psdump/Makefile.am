AM_CPPFLAGS = -I.. -I../.. -I../../.. -I../../layouter

# Prevent any other library to link into this one. 
LIBS =

uidir = @UIDIR@

ui_LTLIBRARIES = libzen_ui_psdump.la
libzen_ui_psdump_la_SOURCES = psdump_render.c psdump_helpers.c \
			     psdump_init.c psdump_open.c psdump_close.c \
			     psdump_text.c psdump_image.c psdump_line.c \
			     psdump_parseAFM.c psdump_parseAFM.h \
			     psdump_font.c psdump_table.c \
			     version.h info.h
#			     psdump_title.c


libzen_ui_psdump_la_LDFLAGS = -version-info 0:1:0

#libzen_ui_psdump_la_LIBADD = @PSDUMP_LIBS@
