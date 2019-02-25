SUBDIRS = . src doc

EXTRA_DIST = STYLES TODO

noinst_HEADERS = paths.h

paths.h: Makefile
	@echo Creating $@
	@rm -f $@ $@.tmp
	@echo '/* Do not edit.  Generated from Makefile. */' > $@.tmp
	@echo >> $@.tmp
	@echo '/* Define to where the user interface shared libraries are installed. */' >> $@.tmp
	@echo '#define UIDIR "$(UIDIR)"' >> $@.tmp
	@chmod 444 $@.tmp
	@mv $@.tmp $@
