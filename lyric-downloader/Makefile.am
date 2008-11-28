
bin_PROGRAMS = lyric-downloader 

common_defines = \
	-D_REENTRANT					\
	-DDBUS_API_SUBJECT_TO_CHANGE			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	-DDATADIR=\""$(datadir)/lmplayer/"\"	\
	-DGCONF_PREFIX=\""/apps/lmplayer"\"		\
	$(DISABLE_DEPRECATED)

# Bacon message connection ltlibrary

lyric_downloader_SOURCES = \
						   http_error_codes.c http_error_codes.h \
						   http_fetcher.c http_fetcher.h \
						   lmp_debug.c lmp_debug.h \
						   lyric_downloader.c lyric_downloader.h \
						   tt_codefunc.c tt_codefunc.h\
						   main.c

lyric_downloader_CPPFLAGS = \
	-I$(top_srcdir)/		\
	-I$(srcdir)/backend		\
	-I$(top_builddir)/data		\
	$(common_defines)		\
	$(AM_CPPFLAGS)

lyric_downloader_CFLAGS = \
	$(WARN_CFLAGS)			\
	$(EXTRA_GNOME_CFLAGS)		\
	$(AM_CFLAGS)

lyric_downloader_LDFLAGS = \
	$(AM_LDFLAGS)

lyric_downloader_LDADD =						\
	$(EXTRA_GNOME_LIBS)				


CLEANFILES =			\
	*.bak			\
	core*			\
	*.orig			\
	*~			\
	$(desktop_DATA)		\
	$(BUILT_SOURCES)

