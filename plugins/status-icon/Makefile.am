modules_flags = -export_dynamic -avoid-version -module

plugindir = $(PLUGINDIR)/status-icon
plugin_LTLIBRARIES = libstatus-icon.la

plugin_in_files = status-icon.lmplayer-plugin.in

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

libstatus_icon_la_SOURCES = lmplayer-status-icon.c
libstatus_icon_la_LDFLAGS = $(modules_flags)
libstatus_icon_la_LIBADD = $(status_icon_LIBS) $(EXTRA_GNOME_LIBS)\
												 $(top_builddir)/src/liblmplayer-main.la \
												 $(top_builddir)/src/backend/libbaconvideowidget.la \
												 $(top_builddir)/liblmplayer-plugin/liblmplayermodule.la \
												 $(top_builddir)/bindings/python/lmplayer-python.la
libstatus_icon_la_CPPFLAGS = $(common_defines)

libstatus_icon_la_CFLAGS = \
	$(DEPENDENCY_CFLAGS)	\
	$(WARN_CFLAGS)		\
	$(DBUS_CFLAGS)		\
	$(AM_CFLAGS)		\
	$(EXTRA_GNOME_CFLAGS) \
	-I$(top_srcdir)/	\
	-I$(top_srcdir)/src	\
	-I$(top_srcdir)/src/backend	\
	-I$(top_srcdir)/liblmplayer-plugin 

#uidir = $(plugindir)
#ui_DATA = status-icon.ui

#imagedir = $(plugindir)
#image_DATA = status-icon.png

EXTRA_DIST = $(plugin_in_files) $(config_DATA) $(ui_DATA) $(image_DATA)

CLEANFILES = $(plugin_DATA) $(BUILT_SOURCES)
DISTCLEANFILES = $(plugin_DATA)

