SUBDIRS = backend

bin_PROGRAMS = lmplayer 
libexec_PROGRAMS =
noinst_LTLIBRARIES = libbaconmessageconnection.la

common_defines = \
	-D_REENTRANT					\
	-DDBUS_API_SUBJECT_TO_CHANGE			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	-DGCONF_PREFIX=\""/apps/lmplayer"\"		\
	-DDATADIR=\""$(datadir)"\"			\
	-DLIBEXECDIR=\""$(libexecdir)"\"		\
	-DBINDIR=\""$(bindir)"\"			\
	-DLOGO_PATH=DATADIR\"\"G_DIR_SEPARATOR_S\"lmplayer\"G_DIR_SEPARATOR_S\"lmplayer_logo.png\"	\
	$(DISABLE_DEPRECATED)

modules_flags = -export_dynamic -avoid-version -module -no-undefined

# Bacon message connection ltlibrary

BACON_MESSAGE_CONNECTION = \
	bacon-message-connection.c \
	bacon-message-connection.h

libbaconmessageconnection_la_SOURCES =	\
	$(BACON_MESSAGE_CONNECTION)

libbaconmessageconnection_la_CPPFLAGS = \
	$(common_defines)	\
	$(AM_CPPFLAGS)

libbaconmessageconnection_la_CFLAGS = \
	$(EXTRA_GNOME_CFLAGS)	\
	$(WARN_CFLAGS)		\
	$(DBUS_CFLAGS)		\
	$(AM_CFLAGS)

libbaconmessageconnection_la_LDFLAGS = \
	$(AM_LDFLAGS)


EGGDIR=$(srcdir)/../../libegg/libegg/recent-files/
BACONDIR=$(srcdir)/../../libbacon/src
regenerate-built-sources:
	EGGFILES="$(BACON_MESSAGE_CONNECTION)" EGGDIR="$(BACONDIR)" $(srcdir)/update-from-egg.sh || true

# Lmplayer UI ltlibrary

# Lmplayer

LMPLAYERMARSHALFILES = 						\
	lmplayerobject-marshal.c lmplayerobject-marshal.h		\
	lmplayerplaylist-marshal.c lmplayerplaylist-marshal.h		

GLIB_GENMARSHAL=`pkg-config --variable=glib_genmarshal glib-2.0`
BUILT_SOURCES = $(LMPLAYERMARSHALFILES)

lmplayerobject-marshal.h: lmplayerobject-marshal.list Makefile
	( $(GLIB_GENMARSHAL) --prefix=lmplayerobject_marshal $(srcdir)/lmplayerobject-marshal.list --header > lmplayerobject-marshal.h )
lmplayerobject-marshal.c: lmplayerobject-marshal.h Makefile
	( $(GLIB_GENMARSHAL) --prefix=lmplayerobject_marshal $(srcdir)/lmplayerobject-marshal.list --header --body > lmplayerobject-marshal.c )
lmplayerplaylist-marshal.h: lmplayerplaylist-marshal.list Makefile
	( $(GLIB_GENMARSHAL) --prefix=lmplayerplaylist_marshal $(srcdir)/lmplayerplaylist-marshal.list --header > lmplayerplaylist-marshal.h )
lmplayerplaylist-marshal.c: lmplayerplaylist-marshal.h Makefile
	( $(GLIB_GENMARSHAL) --prefix=lmplayerplaylist_marshal $(srcdir)/lmplayerplaylist-marshal.list --header --body > lmplayerplaylist-marshal.c )

lmplayer_SOURCES = 					\
	lmplayer.c lmplayer.h				\
	lmplayer-object.c					\
	lmplayer-object.h					\
	lmplayer-remote-command.h\
	$(LMPLAYERMARSHALFILES)				\
	lmplayer-options.c lmplayer-options.h	 \
	lmplayer-playlist.c lmplayer-playlist.h  \
	lmplayer-menu.c lmplayer-menu.h  \
	lmplayer-notebook.c lmplayer-notebook.h  \
	lmplayer-statusbar.c lmplayer-statusbar.h  \
	lmplayer-dvb-setup.c lmplayer-dvb-setup.h\
	lmplayer-interface.c lmplayer-interface.h\
	lmplayer-open-location.c lmplayer-open-location.h\
	lmplayer-encode.c lmplayer-encode.h\
	lmplayer-debug.c lmplayer-debug.h \
	lmplayer-utils.c lmplayer-utils.h \
	lmplayer-uri.c lmplayer-uri.h\
	lmplayer-skin.c lmplayer-skin.h\
	lmplayer-magnetic.c lmplayer-magnetic.h

lmplayer_CPPFLAGS = \
	-I$(top_srcdir)/		\
	-I$(srcdir)/backend		\
	-I$(top_builddir)/data		\
	$(common_defines)		\
	$(AM_CPPFLAGS)

lmplayer_CFLAGS = \
	$(WARN_CFLAGS)			\
	$(EXTRA_GNOME_CFLAGS)		\
	$(DBUS_CFLAGS)			\
	$(NVTV_CFLAGS)			\
	$(MISSING_PLUGINS_CFLAGS) \
	$(AM_CFLAGS)

lmplayer_LDFLAGS = \
	$(AM_LDFLAGS)

lmplayer_LDADD =						\
	backend/libbaconvideowidget.la			\
	libbaconmessageconnection.la			\
	$(EXTRA_GNOME_LIBS)				\
	$(XVIDMODE_LIBS)				\
	$(NVTV_LIBS)					\
	$(DBUS_LIBS)					\
	$(XTEST_LIBS)					\
	$(X_LIBS)

if ENABLE_PYTHON
lmplayer_LDADD += \
	$(PYTHON_LIBS)			\
	$(top_builddir)/bindings/python/lmplayer-python.la
endif

CLEANFILES =			\
	*.bak			\
	core*			\
	*.orig			\
	*~			\
	$(desktop_DATA)		\
	$(BUILT_SOURCES)

EXTRA_DIST = 				\
	lmplayerobject-marshal.list	\
	lmplayerplaylist-marshal.list
		
