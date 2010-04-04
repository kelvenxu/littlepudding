modules_flags = -export_dynamic -avoid-version -module

plugindir = $(PLUGINDIR)/lyric
plugin_LTLIBRARIES = liblyric.la

plugin_in_files = lyric.lmplayer-plugin.in

%.lmplayer-plugin: %.lmplayer-plugin.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

plugin_DATA = $(plugin_in_files:.lmplayer-plugin.in=.lmplayer-plugin) 

common_defines = \
	-D_REENTRANT					\
	-DDBUS_API_SUBJECT_TO_CHANGE			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	-DGCONF_PREFIX=\""/apps/lmplayer"\"		\
	-DDATADIR=\""$(datadir)"\"			\
	-DLIBEXECDIR=\""$(libexecdir)"\"		\
	-DBINDIR=\""$(bindir)"\"			\
	-DLMPLAYER_PLUGIN_DIR=\""$(libdir)/lmplayer/plugins"\"\
	$(DISABLE_DEPRECATED)

liblyric_la_SOURCES = lmplayer-lyric.c
liblyric_la_LDFLAGS = $(modules_flags)
liblyric_la_LIBADD = $(lyric_LIBS) 
liblyric_la_CPPFLAGS = $(common_defines)

liblyric_la_CFLAGS = \
	$(DEPENDENCY_CFLAGS)	\
	$(WARN_CFLAGS)		\
	$(DBUS_CFLAGS)		\
	$(AM_CFLAGS)		\
	$(EXTRA_GNOME_CFLAGS) \
	-I$(top_srcdir)/	\
	-I$(top_srcdir)/src	\
	-I$(top_srcdir)/src/backend	\
	-I$(top_srcdir)/liblmplayer-plugin 

EXTRA_DIST = $(plugin_in_files) 

CLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)
DISTCLEANFILES = $(plugin_DATA)

