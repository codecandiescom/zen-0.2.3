AM_CFLAGS = @MAGICK_CFLAGS@

# Later, the imagemagick include directory will be found in configure.
AM_CPPFLAGS = -I.. -I../.. -I../protocol -I../ui -I../layouter

noinst_LIBRARIES = libimage.a

libimage_a_SOURCES = generic.c helpers.c jpeg.c pngs.c gif.c magick.c \
		     image.h jpeg.h pngs.h gif.h magick.h
