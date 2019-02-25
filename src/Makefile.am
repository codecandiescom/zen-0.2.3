SUBDIRS = common ui parser protocol layouter image

AM_CPPFLAGS = -I. -I.. -Iparser -Ilayouter -Iui -Iprotocol

DEFS = @DEFS@ -DAVAILABLE_UI="\"@AVAILABLE_UI@\"" -DSYSCONFDIR="\"@sysconfdir@\""

bin_PROGRAMS = zen

zen_SOURCES = main.c settings.c retrieve.c threads.c \
	      settings.h retrieve.h threads.h

zen_LDADD = parser/libparser.a layouter/liblayouter.a ui/libui.a \
	    protocol/libprotocol.a image/libimage.a common/libcommon.a 

# We want to install symbolic links for the different
# interface we can provide. The plain text dump interface
# does not get its own symbolic link, and the other two
# only gets them, if they are supported.
install-exec-hook:
	for interface in @AVAILABLE_UI@; do \
	  case "$$interface" in \
	    gtk) ln -sf zen $(bindir)/gzen ;; \
	    ofbis) ln -sf zen $(bindir)/fbzen ;; \
	    psdump) ln -sf zen $(bindir)/pszen ;; \
	  esac ;\
	done

