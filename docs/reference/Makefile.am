## Process this file with automake to produce Makefile.in

# We require automake 1.6 at least.
AUTOMAKE_OPTIONS = 1.6

# This is a blank Makefile.am for using gtk-doc.
# Copy this to your project's API docs directory and modify the variables to
# suit your project. See the GTK+ Makefiles in gtk+/docs/reference for examples
# of using the various options.

# The name of the module, e.g. 'glib'.
DOC_MODULE=lmplayer

# The top-level SGML file. You can change this if you want to.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.xml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting the functions and macros.
# e.g. DOC_SOURCE_DIR=../../../gtk
DOC_SOURCE_DIR=../../src

# Extra options to pass to gtkdoc-scangobj. Not normally needed.
SCANGOBJ_OPTIONS=

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS=--deprecated-guards="GTK_DISABLE_DEPRECATED" 
SCAN_OPTIONS=

# Extra options to supply to gtkdoc-mkdb.
# e.g. MKDB_OPTIONS=--sgml-mode --output-format=xml
MKDB_OPTIONS=--sgml-mode --output-format=xml --name-space=lmplayer

# Extra options to supply to gtkdoc-mktmpl
# e.g. MKTMPL_OPTIONS=--only-section-tmpl
MKTMPL_OPTIONS=

# Extra options to supply to gtkdoc-fixref. Not normally needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS=

MKHTML_OPTIONS=--path="$(abs_builddir)"

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB=$(top_srcdir)/src/*.h
CFILE_GLOB=$(top_srcdir)/src/*.c

# Header files to ignore when scanning.
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h
IGNORE_HFILES = \
	lmplayer-private.h			\
	lmplayerobject-marshal.h		\
	lmplayerplaylist-marshal.h		

# Files we don't want exported to plugins
IGNORE_HFILES += \
	bacon-video-widget-common.h	\
	baconvideowidget-marshal.h	\
	bacon-video-widget-gst-missing-plugins.h

# Files we don't want exported to plugins
IGNORE_HFILES += \
	bacon-video-widget-common.h			\
	baconvideowidget-marshal.h			\
	bacon-video-widget-gst-missing-plugins.h	\
	bacon-resize.h					\
	lmplayer-playlist.h				\
	lmplayer-fullscreen.h				\
	lmplayer-module.h					\
	lmplayer-time-label.h				\
	lmplayer-plugin-manager.h				\
	lmplayer-properties-view.h				\
	lmplayer-open-location.h				\
	lmplayer-python-module.h				\
	lmplayer-statusbar.h				\
	lmplayer-sidebar.h					\
	lmplayer-session.h					\
	lmplayer-resources.h				\
	lmplayer-plugins-engine.h				\
	lmplayer-options.h					\
	lmplayer-preferences.h				\
	lmplayer-dvb-setup.h				\
	lmplayer-uri.h					\
	lmplayer-python-plugin.h				\
	lmplayer-subtitle-encoding.h			\
	lmplayer-menu.h					\
	video-utils.h					\
	lmplayer-utils.h					\
	lmplayer-debug.h						\
	lmplayer-notebook.h						\
	lmplayer-search.h \
	lmplayer-config.h \
	lmplayer-encode.h \
	lmplayer-magnetic.h \
	audio-spectrum.h \
	lmplayer-dnd-menu.h

# Plugin files
IGNORE_HFILES += 

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES=

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.sgml building.sgml changes-2.0.sgml
content_files = version.xml lmplayer-plugins.xml

# SGML files where gtk-doc abbrevations (#GtkWidget) are expanded
# These files must be listed here *and* in content_files
# e.g. expand_content_files=running.sgml
expand_content_files=

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. INCLUDES=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS = \
	-I$(top_srcdir)/		\
	-I$(top_srcdir)/src		\
	-I$(top_srcdir)/src/backend	\
	-I$(top_srcdir)/liblmplayer-plugin	\
	$(DEPENDENCY_CFLAGS)\
	$(EXTRA_GNOME_CFLAGS)

GTKDOC_LIBS = \
	$(EXTRA_GNOME_LIBS) \
	$(DEPENDENCY_LIBS) \
	$(top_srcdir)/src/libbaconmessageconnection.la\
	$(top_srcdir)/src/backend/libbaconvideowidget.la \
	$(top_srcdir)/liblmplayer-plugin/liblmplayermodule.la \
	$(top_srcdir)/bindings/python/lmplayer-python.la \
	$(top_srcdir)/src/liblmplayer-main.la 



# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST += version.xml.in

if ENABLE_GTK_DOC
TESTS_ENVIRONMENT = cd $(srcdir) && 
TESTS = $(GTKDOC_CHECK)
endif
