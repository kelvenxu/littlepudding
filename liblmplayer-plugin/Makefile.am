
noinst_LTLIBRARIES = liblmplayermodule.la

common_defines = \
	-D_REENTRANT					\
	-DDBUS_API_SUBJECT_TO_CHANGE			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	-DGCONF_PREFIX=\""/apps/lmplayer"\"		\
	-DDATADIR=\""$(datadir)"\"			\
	-DLMPLAYER_PLUGIN_DIR=\""$(libdir)/lmplayer/plugins"\" \
	$(DISABLE_DEPRECATED)

modules_flags = -export_dynamic -avoid-version -module

liblmplayermodule_la_SOURCES =		\
	lmplayer-module.c			\
	lmplayer-module.h			\
	lmplayer-plugin.c			\
	lmplayer-plugin.h			\
	lmplayer-plugins-engine.c		\
	lmplayer-plugins-engine.h		\
	lmplayer-plugin-manager.c		\
	lmplayer-plugin-manager.h

liblmplayermodule_la_CPPFLAGS = \
	$(common_defines)	\
	$(AM_CPPFLAGS)

liblmplayermodule_la_CFLAGS = \
	$(AM_CFLAGS)		\
	$(EXTRA_GNOME_CFLAGS) \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/backend	

liblmplayermodule_la_LDFLAGS = \
	$(AM_LDFLAGS)			

if ENABLE_PYTHON

liblmplayermodule_la_LIBADD = \
	$(DEPENDENCY_LIBS)	\
	$(PYTHON_LIBS)		\
	$(PYTHON_EXTRA_LIBS)	\
	$(PYGTK_LIBS)

liblmplayermodule_la_SOURCES += \
	lmplayer-python-module.c		\
	lmplayer-python-module.h		\
	lmplayer-python-plugin.c		\
	lmplayer-python-plugin.h

liblmplayermodule_la_CFLAGS += \
	$(NO_STRICT_ALIASING_CFLAGS)\
	$(PYGTK_CFLAGS)			\
	$(PYTHON_CFLAGS)

endif
