## Process this file with automake to produce Makefile.in

SUBDIRS = icons

stuffdir = $(pkgdatadir)

other_data=\
		   mime-type-include.sh\
		   mime-type-list.txt\
		   mime-functions.sh

stuff_DATA=\
	lmplayer.cfg.example \
	lmplayer.png \
	lmplayer-16x16.png \
	lmplayer-24x24.png \
	lmplayer-32x32.png \
	uri.ui \
	lyricselectdlg.ui \
	lmplayer-library-prefs.ui \
	lmplayer-ui-switch-prefs.ui \
	lmplayer-mini-control.ui \
	plugins.ui

uidir=$(pkgdatadir)/ui
ui_DATA = \
	lmplayer-default.ui \
	lmplayer-classic.ui \
	lmplayer-foobar.ui \
	playlist-view.png \
	playlist-view.svg \
	search-view.png \
	search-view.svg \
	order.png \
	order.svg \
	repeat-one.png \
	repeat-one.svg \
	repeat.png \
	repeat.svg

desktopdir=$(datadir)/applications
desktop_in_files=lmplayer.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

schema_in_files = lmplayer.schemas.in
schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

install-data-local: $(schema_DATA)
if GCONF_SCHEMAS_INSTALL
	if test -z "$(DESTDIR)" ; then \
	for p in $^ ; do \
		GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p 2>&1 > /dev/null; \
	done \
	fi
endif

nodist_noinst_HEADERS = lmplayer-mime-types.h
lmplayer-mime-types.h: mime-type-include.sh mime-type-list.txt mime-functions.sh
	$(srcdir)/mime-type-include.sh $(srcdir)/mime-type-list.txt	\
	> $@

EXTRA_DIST=$(stuff_DATA) $(ui_DATA) $(desktop_in_files) $(other_data) $(schema_in_files)

CLEANFILES = \
	*.bak *.uip core* *.orig *~ 	\
	$(desktop_DATA)			\
	$(generated_schema_in_files)	\
	$(schema_DATA)			\
	lmplayer-handlers.schemas.in 	\
	lmplayer-mime-types.h		\
	update-gconf.sh
