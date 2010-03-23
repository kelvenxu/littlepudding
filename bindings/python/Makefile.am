## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = lmplayer-python.la

nodist_lmplayer_python_la_SOURCES = lmplayer.c

lmplayer_python_la_SOURCES = override_common.c override_common.h

lmplayer_python_la_LDFLAGS = \
	-module -avoid-version

lmplayer_python_la_LIBADD = \
	$(DEPENDENCY_LIBS)	\
	$(PYTHON_LIBS)		\
	$(PYTHON_EXTRA_LIBS)	\
	$(PYGTK_LIBS)

lmplayer_python_la_CFLAGS = \
	-I$(top_srcdir)			\
	-I$(top_srcdir)/src		\
	-I$(top_srcdir)/src/backend	\
	-I$(top_srcdir)/liblmplayer-plugin	\
	$(DEPENDENCY_CFLAGS)		\
	$(EXTRA_GNOME_CFLAGS) \
	$(PYGTK_CFLAGS)			\
	$(PYTHON_CFLAGS)		\
	$(AM_CFLAGS)			\
	$(WNOERROR_CFLAGS)

lmplayer.c: lmplayer.defs lmplayer.override
	( cd $(srcdir) && $(PYGTK_CODEGEN) \
		--register $(PYGTK_DEFSDIR)/gtk-types.defs \
		--override $*.override \
		--prefix py$* $(<F) ) > $@ 

BINDING_HEADERS_SRCDIR_IN = \
	src/lmplayer-object.h				\
	src/lmplayer.h				\
	src/lmplayer-remote-command.h				\
	liblmplayer-plugin/lmplayer-plugin.h		\
	$(NULL)

BINDING_HEADERS_BUILDDIR_IN = 

BINDING_HEADERS_SRCDIR		:= $(addprefix $(top_srcdir)/,$(BINDING_HEADERS_SRCDIR_IN))
BINDING_HEADERS_BUILDDIR	:= $(addprefix $(top_builddir)/,$(BINDING_HEADERS_BUILDDIR_IN))

regenerate-python-binding:
	$(PYGTK_H2DEF) $(sort $(BINDING_HEADERS_SRCDIR) $(BINDING_HEADERS_BUILDDIR)) > lmplayer.defs.new

BUILT_SOURCES = lmplayer.c

EXTRA_DIST = \
	lmplayer.override		\
	lmplayer.defs

CLEANFILES = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

