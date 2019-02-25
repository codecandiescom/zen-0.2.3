AM_CPPFLAGS = -I.. -I../.. -I../../.. -I../../layouter -I../progress_logo @GTK_CFLAGS@

# Prevent any other library to link into this one. 
# But we want to link the debug library into this. Maybe not.
LIBS = # @DEBUG_LIBS@

uidir = @UIDIR@

ui_LTLIBRARIES = libzen_ui_gtk.la
libzen_ui_gtk_la_SOURCES = gtk_init.c gtk_open.c gtk_close.c gtk_main.c \
			   gtk_helpers.c gtk_font.c gtk_render.c gtk_form.c \
			   gtk_text.c gtk_line.c gtk_table.c gtk_image.c \
			   version.h info.h

libzen_ui_gtk_la_LDFLAGS = -version-info 1:0:1

libzen_ui_gtk_la_LIBADD = @GTK_LIBS@
