## Process this file with automake to produce Makefile.in

SUBDIRS = icons

stuffdir = $(pkgdatadir)

other_data=\
		   mime-type-include.sh\
		   mime-type-list.txt\
		   mime-functions.sh

stuff_DATA=\
		   lmplayer.cfg.example\
		   lmplayer.png\
		   lmplayer-16x16.png\
		   lmplayer-24x24.png\
		   lmplayer-32x32.png\
		   uri.ui\
		   lyricselectdlg.ui

desktopdir=$(datadir)/applications
desktop_in_files=lmplayer.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

nodist_noinst_HEADERS = lmplayer-mime-types.h
lmplayer-mime-types.h: mime-type-include.sh mime-type-list.txt mime-functions.sh
	$(srcdir)/mime-type-include.sh $(srcdir)/mime-type-list.txt	\
	> $@

EXTRA_DIST=$(stuff_DATA) $(desktop_in_files) $(other_data)

CLEANFILES = \
	*.bak *.uip core* *.orig *~ 	\
	$(desktop_DATA)			\
	$(generated_schema_in_files)	\
	$(schema_DATA)			\
	lmplayer-handlers.schemas.in 	\
	lmplayer-mime-types.h		\
	update-gconf.sh
