ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs \
		       ABOUT-NLS compile

SUBDIRS = src po

EDJE_FLAGS = -v \
	     -id $(top_srcdir)/images

filesdir = $(datadir)
files_DATA = desktitle.edj module.desktop e-module-desktitle.edj

EXTRA_DIST = module.desktop.in \
	     e-module-desktitle.edc \
	     desktitle.edc \
	     images/module_icon.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	rm -rf desktitle.edj e-module-desktitle.edj module.desktop

uninstall:
	rm -rf $(DESTDIR)$(datadir)
