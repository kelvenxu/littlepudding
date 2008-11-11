noinst_PROGRAMS = \
				   mytest

common_ldflags =

mytest_SOURCES = test.c
mytest_LDADD = $(common_ldflags)\
			   $(PACKAGE_LIBS)

INCLUDES =\
		  -I$(top_srcdir) \
		  $(PACKAGE_CFLAGS)\
		  $(CFLAGS)
