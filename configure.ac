AC_PREREQ(2.52)

m4_define(lmplayer_version_major, 0)
m4_define(lmplayer_version_minor, 10)
m4_define(lmplayer_version_micro, 0)

AC_INIT([lmplayer],
        [lmplayer_version_major.lmplayer_version_minor.lmplayer_version_micro],
        [http://www.lmplayer.org])

AC_CONFIG_SRCDIR([src/lmplayer.c])
AC_CONFIG_HEADERS([config.h:config.in])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 no-dist-gzip check-news subdir-objects])

dnl Add the languages which your application supports to po/LINGUAS
GETTEXT_PACKAGE=lmplayer
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define to the Gettext package name])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

GNOME_DOC_INIT

AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
m4_ifdef([LT_OUTPUT], [LT_OUTPUT])

PKG_PROG_PKG_CONFIG

dnl AC_CHECK_FUNCS(asprintf)
dnl AC_CONFIG_LIBOBJ_DIR([lib])
dnl AC_REPLACE_FUNCS([asprintf])

dnl AC_PATH_PROG([GLIB_GENMARSHAL],[glib-genmarshal])
dnl AC_PATH_PROG([GLIB_MKENUMS],[glib-mkenums])

# Requirements
GLIB_REQS=2.13.4
GTK_REQS=2.12.0
GNOMEICON_REQS=2.15.90
TOTEM_PLPARSER_REQS=2.23.91

LMPLAYER_VERSION_MAJOR=lmplayer_version_major
LMPLAYER_VERSION_MINOR=lmplayer_version_minor
LMPLAYER_VERSION_MICRO=lmplayer_version_micro
AC_SUBST(LMPLAYER_VERSION_MAJOR)
AC_SUBST(LMPLAYER_VERSION_MINOR)
AC_SUBST(LMPLAYER_VERSION_MICRO)

PLPARSER_LT_VERSION=12:2:0
AC_SUBST(PLPARSER_LT_VERSION)

LMPLAYER_PL_PARSER_VERSION_MAJOR=lmplayer_version_major
LMPLAYER_PL_PARSER_VERSION_MINOR=lmplayer_version_minor
LMPLAYER_PL_PARSER_VERSION_MICRO=lmplayer_version_micro
AC_SUBST(LMPLAYER_PL_PARSER_VERSION_MAJOR)
AC_SUBST(LMPLAYER_PL_PARSER_VERSION_MINOR)
AC_SUBST(LMPLAYER_PL_PARSER_VERSION_MICRO)

PLUGINDIR='${libdir}/lmplayer/plugins'
AC_SUBST(PLUGINDIR)

AC_ARG_ENABLE(nvtv, AC_HELP_STRING([--enable-nvtv],[Enable support for TV-output with nvtv]), enable_nvtv="$enableval", enable_nvtv=yes)

if test x$enable_nvtv = xyes; then
PKG_CHECK_MODULES(NVTV, nvtvsimple >= 0.4.5, found_nvtv=yes, found_nvtv=no)
	if test x"$found_nvtv" = "xyes" ; then
		AC_DEFINE(HAVE_NVTV,1,[Define to 1 if nvtv is available and used.])
	fi
fi

dnl xine or gst ?

dnl the two versions here implicate the gstreamer core and gst-plugins
dnl release versions.
dnl XINE_REQS=1.1.7
GST_MAJORMINOR=0.10
GST_REQS=0.10.12
GSTPLUG_REQS=0.10.12

dnl ***
dnl HAL
dnl ***

AC_ARG_WITH(hal,
	      AC_HELP_STRING([--without-hal],
			     [Disable HAL support]))
if test "x$with_hal" != "xno"; then
  PKG_CHECK_MODULES(HAL, hal >= 0.5 hal < 0.6 dbus-1 >= $DBUS_REQS, enable_hal05=yes, enable_hal05=no)
  if test x$enable_hal05 = xyes; then
      with_hal=yes
      AC_DEFINE(HAVE_HAL, 1, [Define if you HAL support])
  else
    if test "x$with_hal" = "xyes"; then
	  AC_MSG_ERROR([HAL support explicitly requested but HAL couldn't be found])
    fi
  fi
fi
AM_CONDITIONAL(HAVE_HAL, test x"$with_hal" = xyes)

dnl Check for packages for building liblmplayer-plparser.la
dnl PKG_CHECK_MODULES(LMPLAYER_PLPARSER, [glib-2.0 >= $GLIB_REQS gtk+-2.0 libxml-2.0 camel-1.2 gio-2.0])

dnl PKG_CHECK_MODULES([LMPLAYER_PLPARSER_MINI], [gio-2.0])


dnl AC_ARG_ENABLE(xine,
dnl 	AC_HELP_STRING([--enable-xine], [use xine instead of GStreamer for the backend]),
dnl 	[case "${enableval}" in
dnl 	yes) ENABLE_XINE=yes ;;
dnl         no)  ENABLE_XINE=no ;;
dnl 	*) AC_MSG_ERROR([
dnl              ***** Bad value ${enableval} for --enable-xine
dnl              ***** Please use one of the following:
dnl              *****   --enable-xine=yes
dnl 	     *****   --enable-xine=no
dnl            ]) ;;
dnl 	esac],
dnl 	[ENABLE_XINE=auto]) dnl Default value

ENABLE_XINE=no
HAVE_GSTREAMER=no
if test x$ENABLE_XINE = xno || test x$ENABLE_XINE = xauto; then
	dnl Check for the required GStreamer versions for missing plugins
	dnl installation, unless this has been disabled.
	AC_MSG_CHECKING([whether to enable easy codec installation support])
	AC_ARG_ENABLE([easy-codec-installation],
		[
			AS_HELP_STRING([--enable-easy-codec-installation],
				[Whether to enable easy codec installation support for GStreamer]
			)
		], [
			case "${enableval}" in
				yes) enable_easy_codec_installation=yes ;;
				no)  enable_easy_codec_installation=no ;;
				*) enable_easy_codec_installation=auto ;;
			esac
		], [
			dnl default value
			enable_easy_codec_installation=auto
		]
	)
	AC_MSG_RESULT([$enable_easy_codec_installation])

	if test "x$enable_easy_codec_installation" != "xno"; then
		PKG_CHECK_MODULES(MISSING_PLUGINS, gstreamer-plugins-base-0.10,
		[
			MISSING_PLUGINS_LIBS="$MISSING_PLUGINS_LIBS -lgstpbutils-0.10"
			AC_DEFINE([ENABLE_MISSING_PLUGIN_INSTALLATION], 1,
			          [Whether we can and want to do installation of missing plugins])
		])
	fi

	MM="gstreamer-0.10 >= $GST_REQS gstreamer-base-0.10 >= $GST_REQS gstreamer-plugins-base-0.10 >= $GSTPLUG_REQS gstreamer-tag-0.10 >= $GSTPLUG_REQS gconf-2.0"
	PKG_CHECK_MODULES(GST, $MM,
			HAVE_GSTREAMER=yes, HAVE_GSTREAMER=no)
	GST_LIBS="$GST_LIBS -lgstbase-$GST_MAJORMINOR -lgstinterfaces-$GST_MAJORMINOR -lgstvideo-$GST_MAJORMINOR -lgstaudio-$GST_MAJORMINOR -lgstpbutils-$GST_MAJORMINOR -lgsttag-$GST_MAJORMINOR"
	AC_SUBST(GST_LIBS)
	EXTRA_BACKEND_LIBS="-lgstinterfaces-$GST_MAJORMINOR -lgstvideo-$GST_MAJORMINOR -lgstaudio-$GST_MAJORMINOR -lgstpbutils-$GST_MAJORMINOR"

	AC_MSG_CHECKING(for backend libraries)
	dnl Give error and exit if we don't have gstreamer
	if test "x$HAVE_GSTREAMER" = "xno"; then
		AC_MSG_ERROR(you need the GStreamer or the xine-lib development packages installed)
	fi
	AC_MSG_RESULT(GStreamer-$GST_MAJORMINOR)

	dnl Check for required plugins
	gst010_toolsdir=`$PKG_CONFIG --variable=toolsdir gstreamer-0.10`
	gst010_inspect="$gst010_toolsdir/gst-inspect-0.10"

	dnl Check for elements from gst-plugins-base
	for base_element in playbin ffmpegcolorspace videoscale
	do
		AC_MSG_CHECKING([GStreamer 0.10 $base_element plugin])
		if $gst010_inspect $base_element >/dev/null 2>/dev/null; then
			AC_MSG_RESULT([yes])
		else
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([
				Cannot find required GStreamer-0.10 plugin '$base_element'.
				It should be part of gst-plugins-base. Please install it.
			])
		fi
	done

	dnl Check for elements from gst-plugins-good
	for good_element in gconfaudiosink gconfvideosink goom
	do
		AC_MSG_CHECKING([GStreamer 0.10 $good_element plugin])
		if $gst010_inspect $good_element >/dev/null 2>/dev/null; then
			AC_MSG_RESULT([yes])
		else
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([
				Cannot find required GStreamer-0.10 plugin '$good_element'.
				It should be part of gst-plugins-good. Please install it.
			])
		fi
	done


fi

if test x$ENABLE_XINE = xno && test x$HAVE_GSTREAMER = xno; then
	AC_MSG_CHECKING(for backend libraries)
	AC_MSG_RESULT([none])
	AC_MSG_ERROR(you need the GStreamer or the xine-lib development packages installed)
fi

if test "$HAVE_GSTREAMER" = "yes"; then
	AC_DEFINE([HAVE_GSTREAMER],[1],[Define if using gst backend])
fi

AM_CONDITIONAL(LMPLAYER_GST, test "x$HAVE_GSTREAMER" = "xyes")

AC_MSG_CHECKING([Whether not to check for iso-codes])
AC_ARG_ENABLE([iso-codes],
	AS_HELP_STRING([--disable-iso-codes],[Whether not to check for iso-codes at build-time]),
	[],[disable_iso_codes_check=no])
if test x$disable_iso_codes_check = xno ; then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
fi

if test x$disable_iso_codes_check = "xno" ; then
	AC_MSG_CHECKING([whether iso-codes has iso-639 domain])
	if $PKG_CONFIG --variable=domains iso-codes | grep 639 >/dev/null ; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
	fi
	AC_DEFINE_UNQUOTED([ISO_CODES_PREFIX],["`$PKG_CONFIG --variable=prefix iso-codes`"],[ISO codes prefix])
	ISO_CODES=iso-codes
else
	AC_DEFINE_UNQUOTED([ISO_CODES_PREFIX],["$prefix"],[ISO codes prefix])
	ISO_CODES=""
fi

dnl Seems that the order matters because libtool blows
dnl Also check if we're going to be using GTK+ only

PKG_CHECK_MODULES(EXTRA_GNOME, 
									glib-2.0 >= $GLIB_REQS 
									gtk+-2.0 >= $GTK_REQS 
									gio-2.0 
									gnome-icon-theme >= $GNOMEICON_REQS 
									totem-plparser >= $TOTEM_PLPARSER_REQS
									gconf-2.0
									gmodule-2.0 
									liblmplayer-search >= 0.10.0
									liblmplayer-lyric >= 0.10.0
									unique-1.0
									$ISO_CODES)

EXTRA_GNOME_CFLAGS="$EXTRA_GNOME_CFLAGS"
EXTRA_GNOME_LIBS="$EXTRA_GNOME_LIBS $EXTRA_BACKEND_LIBS"
AC_SUBST(EXTRA_GNOME_CFLAGS)
AC_SUBST(EXTRA_GNOME_LIBS)

dnl *************************
dnl X11 related functionality
dnl *************************
have_x11=no
if test x$(pkg-config --variable=target gtk+-2.0) = xx11; then
	AC_PATH_X

	have_x11=yes

	if test x"$x_includes" != x"NONE" && test -n "$x_includes" ; then
		X_INCLUDES=-I`echo $x_includes | sed -e "s/:/ -I/g"`
	fi
	if test x"$x_libraries" != x"NONE" && test -n "$x_libraries" ; then
		X_LIBRARIES=-L`echo $x_libraries | sed -e "s/:/ -L/g"`
	fi
	CFLAGS="$X_INCLUDES $CFLAGS"
	LIBS="$X_LIBRARIES $LIBS"

	XTEST_LIBS=""
	enable_xtest=no
	AC_CHECK_LIB(Xtst, XTestFakeKeyEvent,
		XTEST_LIBS="-lX11 -lXtst $X_LIBRARIES"
		enable_xtest=yes
		AC_DEFINE(HAVE_XTEST, 1, [defined if you have XTest library]),
		,
		$X_LIBRARIES)
	AC_SUBST(XTEST_LIBS)

	PKG_CHECK_MODULES(XVIDMODE, xrandr >= 1.1.1 xxf86vm >= 1.0.1,
		have_xvidmode=yes, have_xvidmode=no)

	if test x$have_xvidmode = xyes; then
		AC_DEFINE(HAVE_XVIDMODE,, [Define this if you have the XVidMode and XRandR extension installed])
	fi

	X_LIBS=""

	dnl Explicitely link against libX11 to avoid problems with crappy linkers
	X_LIBS="$X_LIBRARIES -lX11"
	AC_SUBST(X_LIBS)

	dnl Multimedia keys
	have_xfree=no
	AC_COMPILE_IFELSE([
		#include <X11/XF86keysym.h>
		int main(int argc,char **argv) {
		return 0;
		}
		],
		have_xfree=yes
	)
	AC_MSG_CHECKING(for X11 XFree86 headers)
	AC_MSG_RESULT([$have_xfree])
	if test x"$have_xfree" = "xyes" ; then
		AC_DEFINE(HAVE_XFREE, 1, [defined if you have X11/XF86keysym.h])
	fi
fi
AM_CONDITIONAL(HAVE_XVIDMODE, [test x$have_xvidmode = xyes])

dnl ================================================================
dnl Python plugins
dnl ================================================================

AC_MSG_CHECKING([whether Python plugin support is requested])
AC_ARG_ENABLE([python],
	AS_HELP_STRING([--enable-python],[Enable python support]),
	[enable_python=$enableval have_python=$enableval],
	[enable_python=autodetect have_python=yes])
AC_MSG_RESULT([$enable_python])

if test "x$have_python" != "xyes"; then
	if test "x$enable_python" = "xyes"; then
		AC_MSG_ERROR([Python not found])
	elif test "x$enable_python" = "xautodetect"; then
		enable_python=no
		AC_MSG_WARN([Python not found, disabling python support])
	fi
fi

if test "x$have_python" != "xno"; then
	AM_PATH_PYTHON([2.3],[],[have_python=no])
	if test "x$PYTHON" = "x:"; then
		have_python=no
	fi
fi

if test "x$have_python" != "xno"; then
	PY_PREFIX=`$PYTHON -c 'import sys ; print sys.prefix'`
	PY_EXEC_PREFIX=`$PYTHON -c 'import sys ; print sys.exec_prefix'`
	PYTHON_LIBS="-lpython$PYTHON_VERSION"
	if test -d $PY_EXEC_PREFIX/lib64/python$PYTHON_VERSION ; then
		PYTHON_LIB_LOC="-L$PY_EXEC_PREFIX/lib64/python$PYTHON_VERSION/config"
		PYTHON_MAKEFILE="$PY_EXEC_PREFIX/lib64/python$PYTHON_VERSION/config/Makefile"
	else
		PYTHON_LIB_LOC="-L$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config"
		PYTHON_MAKEFILE="$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config/Makefile"
	fi
	PYTHON_CFLAGS="-I$PY_PREFIX/include/python$PYTHON_VERSION"
	PYTHON_LOCALMODLIBS=`sed -n -e 's/^LOCALMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_BASEMODLIBS=`sed -n -e 's/^BASEMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_OTHER_LIBS=`sed -n -e 's/^LIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_EXTRA_LIBS="$PYTHON_LOCALMODLIBS $PYTHON_BASEMODLIBS $PYTHON_OTHER_LIBS"
	AC_SUBST([PYTHON_LIBS])
	AC_SUBST([PYTHON_LIB_LOC])
	AC_SUBST([PYTHON_CFLAGS])
	AC_SUBST([PYTHON_EXTRA_LIBS])

	PYGTK_REQUIRED=2.12.0

	PKG_CHECK_MODULES([PYGTK], [pygtk-2.0 >= $PYGTK_REQUIRED],
		[],
		[
		have_python=no
		if test "x$enable_python" = "xyes"; then
			AC_MSG_ERROR([$PYGTK_PKG_ERRORS])
		elif test "x$enable_python" = "xautodetect"; then
			enable_python=no
			AC_MSG_WARN([$PYGTK_PKG_ERRORS])
			AC_MSG_WARN([Disabling python support])
		fi
		])

	AC_SUBST([PYGTK_CFLAGS])
	AC_SUBST([PYGTK_LIBS])
fi

if test "x$have_python" != "xno"; then
	AC_MSG_CHECKING([for pygtk defs])
	PYGTK_DEFSDIR=`$PKG_CONFIG --variable=defsdir pygtk-2.0`
	AC_MSG_RESULT([$PYGTK_DEFSDIR])

	AC_MSG_CHECKING([for pygtk codegen])
	PYGTK_CODEGEN="$PYTHON `$PKG_CONFIG --variable=codegendir pygtk-2.0`/codegen.py"
	AC_MSG_RESULT([$PYGTK_CODEGEN])

	AC_MSG_CHECKING([for pygtk h2def])
	PYGTK_H2DEF="$PYTHON `$PKG_CONFIG --variable=codegendir pygtk-2.0`/h2def.py"
	AC_MSG_RESULT([$PYGTK_H2DEF])

	AC_SUBST([PYGTK_DEFSDIR])
	AC_SUBST([PYGTK_CODEGEN])
	AC_SUBST([PYGTK_H2DEF])

dnl	uncomment when http://bugzilla.gnome.org/show_bug.cgi?id=351072 fixed
dnl	PKG_CHECK_EXISTS([pygobject-2.0 >= X.XX.X],
dnl			 AC_DEFINE([PYGOBJECT_CAN_MARSHAL_GVALUE]))

	dnl Check for -fno-strict-aliasing
	FLAGS="-fno-strict-aliasing"
	save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $FLAGS"
	AC_MSG_CHECKING([whether [$]CC understands $FLAGS])
	AC_TRY_COMPILE([], [], [compiler_has_option=yes], [compiler_has_option=no])
	CFLAGS="$save_CFLAGS"
	AC_MSG_RESULT($compiler_has_option)
	if test $compiler_has_option = yes; then
		NO_STRICT_ALIASING_CFLAGS="$FLAGS"
	fi
	AC_SUBST([NO_STRICT_ALIASING_CFLAGS])
fi

if test "x$have_python" != "xno" -a "x$enable_python" != "xno"; then
	enable_python=yes
	AC_DEFINE([ENABLE_PYTHON],[1],[Define to compile with python plugin support])
fi

AM_CONDITIONAL([ENABLE_PYTHON],[test "x$enable_python" = "xyes"])

dnl ********
dnl Nautilus
dnl ********

dnl Check for Nautilus property page build
AC_ARG_ENABLE(nautilus,
	AC_HELP_STRING([--enable-nautilus],[compile the nautilus plugin]),
	[case "${enableval}" in
	yes) ENABLE_NAUTILUS=yes ;;
	no) ENABLE_NAUTILUS=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-nautilus) ;;
	esac],
	[ENABLE_NAUTILUS=yes]) dnl Default value

if test x$ENABLE_NAUTILUS = "xyes" ; then
	PKG_CHECK_MODULES(NAUTILUS, gtk+-2.0 gthread-2.0 libnautilus-extension,
			[HAVE_NAUTILUS=yes], [HAVE_NAUTILUS=no])
fi

NAUTILUS_LIBS="$NAUTILUS_LIBS $EXTRA_BACKEND_LIBS"
AC_SUBST(NAUTILUS_CFLAGS)
AC_SUBST(NAUTILUS_LIBS)
if test x$HAVE_NAUTILUS = "xyes"; then
	AC_DEFINE(HAVE_NAUTILUS, 1, [defined if you build the nautilus plugin])
	NAUTILUSDIR=`pkg-config --variable=extensiondir libnautilus-extension`
	AC_SUBST(NAUTILUSDIR)
fi
AM_CONDITIONAL(HAVE_NAUTILUS, test x$HAVE_NAUTILUS = "xyes")

dnl ***************************
dnl D-Bus for gnome-screensaver
dnl ***************************
AC_ARG_WITH(dbus,
		AC_HELP_STRING([--with-dbus],
			[Enable D-BUS support]),,
		with_dbus=auto)
if test "x$with_dbus" != "xno"; then
	PKG_CHECK_MODULES(DBUS, dbus-glib-1 >= $DBUS_REQS, enable_dbus=yes, enable_dbus=no)
        if test "x$with_dbus" = xyes && test "x$enable_dbus" = xno; then
	   AC_MSG_ERROR([D-BUS explicitly requested but no support found])
	fi
        if test "x$enable_dbus" = xyes; then
		AC_DEFINE(WITH_DBUS, 1, [Define if D-BUS is enabled])
	fi
fi
AM_CONDITIONAL(WITH_DBUS, test "x$enable_dbus" = "xyes")

AC_PATH_PROG(GCONFTOOL, gconftool-2)
AM_GCONF_SOURCE_2

GNOME_COMMON_INIT
GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])
GNOME_CXX_WARNINGS
GNOME_MAINTAINER_MODE_DEFINES

GTK_DOC_CHECK(1.11)

# This macro expands DIR and assigns it to RET.
# If DIR is NONE, then it's replaced by DEFAULT.
# Based on AC_DEFINE_DIR
AC_DEFUN([LMPLAYER_FULLPATH], [
  test "x$prefix" = xNONE && prefix="$ac_default_prefix"
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  ac_expand=[$]$1
  test "x$ac_expand" = xNONE && ac_expand="[$]$2"
  ac_expand=`eval echo [$]ac_expand`
  $3=`eval echo [$]ac_expand`
])

dnl FULL_LIBEXECDIR is used for X-GNOME-Bugzilla-ExtraInfoScript expansion
dnl in data/lmplayer.desktop.in.in.in
LMPLAYER_FULLPATH(libexecdir, NONE, FULL_LIBEXECDIR)
AC_SUBST(FULL_LIBEXECDIR)

dnl debug
AC_ARG_ENABLE(debug,
		AC_HELP_STRING([--disable-debug],[disables compilation of debugging messages]),
	[case "${enableval}" in
	yes) ENABLE_DEBUG=yes ;;
	no)  ENABLE_DEBUG=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-debug) ;;
	esac],
[ENABLE_DEBUG=no]) dnl Default value
if test x$ENABLE_DEBUG = xyes; then
	AC_DEFINE(LMPLAYER_DEBUG, 1, [Define if DEBUG statements should be compiled in])
fi

dnl run in source tree
AC_ARG_ENABLE(run-in-source-tree,
	      AC_HELP_STRING([--disable-run-in-source-tree],[disable search for UI files and plugins in local directory ]),
	[case "${enableval}" in
	yes) ENABLE_RUN_IN_SOURCE_TREE=yes ;;
	no)  ENABLE_RUN_IN_SOURCE_TREE=no ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-run-in-source-tree) ;;
	esac],
[ENABLE_RUN_IN_SOURCE_TREE=yes]) dnl Default value
if test x$ENABLE_RUN_IN_SOURCE_TREE = xyes; then
	AC_DEFINE(LMPLAYER_RUN_IN_SOURCE_TREE, 1, [enable search for UI files and plugins in local directory])
fi

if test "$GCC" = "yes" ; then
	CFLAGS="\
	-Wall \
	-Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes \
	-Wnested-externs -Wpointer-arith \
	-Wcast-align -Wsign-compare \
	-fno-strict-aliasing \
	$CFLAGS"
fi

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])


AC_OUTPUT([
Makefile
src/backend/Makefile
src/Makefile
liblmplayer-plugin/Makefile
lyric-downloader/Makefile
data/Makefile
data/lmplayer.desktop.in
data/icons/Makefile
data/icons/hicolor/Makefile
data/icons/hicolor/16x16/Makefile
data/icons/hicolor/16x16/apps/Makefile
data/icons/hicolor/22x22/Makefile
data/icons/hicolor/22x22/apps/Makefile
data/icons/hicolor/24x24/Makefile
data/icons/hicolor/24x24/apps/Makefile
data/icons/hicolor/32x32/Makefile
data/icons/hicolor/32x32/apps/Makefile
data/icons/hicolor/48x48/Makefile
data/icons/hicolor/48x48/apps/Makefile
data/icons/hicolor/scalable/Makefile
data/icons/hicolor/scalable/apps/Makefile
plugins/Makefile
plugins/lmplayer/Makefile
plugins/rereading/Makefile
bindings/Makefile
bindings/python/Makefile
tests/Makefile
po/Makefile.in
docs/Makefile
docs/reference/Makefile
docs/reference/version.xml
])

AC_MSG_NOTICE([Lmplayer was configured with the following options:])
if test x$HAVE_GSTREAMER = xno ; then
	AC_MSG_NOTICE([** Using the xine backend])
else
	AC_MSG_NOTICE([** Using the GStreamer-$GST_MAJORMINOR backend])
	if test "x$MISSING_PLUGINS_LIBS" != "x"; then
		AC_MSG_NOTICE([** Easy codec installation support enabled])
	else
		AC_MSG_NOTICE([   Easy codec installation support disabled])
	fi
fi
if test x$found_nvtv = xyes ; then
	AC_MSG_NOTICE([** nvtv support enabled])
else
	AC_MSG_NOTICE([   nvtv support disabled])
fi
if test x$ENABLE_GTK = xyes ; then
	AC_MSG_NOTICE([** GTK+-only version enabled])
else
	AC_MSG_NOTICE([** GNOME version enabled])
fi
if test x$enable_python = xyes ; then
	AC_MSG_NOTICE([** Python binding support enabled])
else
	AC_MSG_NOTICE([   Python binding support disabled])
fi
if test x$with_vala = xyes ; then
	AC_MSG_NOTICE([** Vala binding support enabled])
else
	AC_MSG_NOTICE([   Vala binding support disabled])
fi
if test "x${PLUGINS}" != "x" ; then
	for allowed_plugin in ${ALL_PLUGINS}; do
		for plugin in ${PLUGINS}; do
			case ${allowed_plugin} in
				${plugin})
					AC_MSG_NOTICE([** ${allowed_plugin} plugin enabled])
					continue 2
				;;
				*);;
			esac
		done
		AC_MSG_NOTICE([   ${allowed_plugin} plugin disabled])
	done
else
	AC_MSG_NOTICE([   No Lmplayer plugins enabled])
fi
if test x$enable_browser_plugins = xyes ; then
	AC_MSG_NOTICE([** Browser plugin enabled (using $MOZILLA)])
	if test x$enable_basic_plugin = xyes ; then
		AC_MSG_NOTICE([** Basic browser plugin enabled])
	else
		AC_MSG_NOTICE([   Basic browser plugin disabled])
	fi
	if test x$enable_gmp_plugin = xyes ; then
		AC_MSG_NOTICE([** GMP (Windows Media) plugin enabled])
	else
		AC_MSG_NOTICE([   GMP (Windows Media) plugin disabled])
	fi
	if test x$enable_complex_plugin = xyes ; then
		AC_MSG_NOTICE([** Complex (Real) plugin enabled])
	else
		AC_MSG_NOTICE([   Complex (Real) plugin disabled])
	fi
	if test x$enable_narrowspace_plugin = xyes ; then
		AC_MSG_NOTICE([** NarrowSpace (QuickTime) plugin enabled])
	else
		AC_MSG_NOTICE([   NarrowSpace (QuickTime) plugin disabled])
	fi
	if test x$enable_mully_plugin = xyes ; then
		AC_MSG_NOTICE([** MullY (DivX) plugin enabled])
	else
		AC_MSG_NOTICE([   MullY (DivX) plugin disabled])
	fi
	if test x$enable_cone_plugin = xyes ; then
		AC_MSG_NOTICE([** Cone (VLC) plugin enabled])
	else
		AC_MSG_NOTICE([   Cone (VLC) plugin disabled])
	fi
else
	AC_MSG_NOTICE([   Browser plugin disabled])
fi
if test x$ENABLE_NAUTILUS = xyes ; then
	AC_MSG_NOTICE([** Nautilus properties page enabled])
else
	AC_MSG_NOTICE([   Nautilus properties page disabled])
fi
if test x$disable_iso_codes_check = xno ; then
	AC_MSG_NOTICE([** iso-codes support checked])
else
	AC_MSG_NOTICE([   iso-codes support checked at run-time])
fi
if test x$enable_xtest = xyes ; then
	AC_MSG_NOTICE([** XTest (legacy screensaver) support enabled])
else
	AC_MSG_NOTICE([   XTest (legacy screensaver) support disabled])
fi
if test x$enable_dbus = xyes ; then
	AC_MSG_NOTICE([** D-Bus (gnome-screensaver) support enabled])
else
	AC_MSG_NOTICE([   D-Bus (gnome-screensaver) support disabled])
fi
if test x$have_xvidmode = xyes ; then
	AC_MSG_NOTICE([** XVidmode support enabled])
else
	AC_MSG_NOTICE([   XVidmode support disabled])
fi
if test x$have_xfree = xyes ; then
	AC_MSG_NOTICE([** XFree86 multimedia keys support enabled])
else
	AC_MSG_NOTICE([   XFree86 multimedia keys support disabled])
fi

AC_MSG_NOTICE([End options])

