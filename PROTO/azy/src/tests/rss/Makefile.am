AM_CFLAGS= \
  $(AZY_CFLAGS) \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include
AM_CXXFLAGS = $(AM_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in

check_PROGRAMS = rss
rss_SOURCES = rss.c
rss_LDADD = \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la
