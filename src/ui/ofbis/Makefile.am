AM_CPPFLAGS = -I.. -I../.. -I../../.. -I../../layouter @OFBIS_CFLAGS@ -DUSE_CONTROL_FIELD

# Prevent any other library to link into this one. 
LIBS =

uidir = @UIDIR@

ui_LTLIBRARIES = libzen_ui_ofbis.la
libzen_ui_ofbis_la_SOURCES = ofbis_main.c ofbis_render.c ofbis_helpers.c \
			     ofbis_init.c ofbis_open.c ofbis_close.c \
			     ofbis_text.c ofbis_image.c ofbis_line.c \
			     ofbis_title.c ofbis_table.c \
			     version.h info.h

libzen_ui_ofbis_la_LDFLAGS = -version-info 0:2:0

libzen_ui_ofbis_la_LIBADD = @OFBIS_LIBS@
