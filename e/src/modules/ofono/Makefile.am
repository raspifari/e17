MAINTAINERCLEANFILES = Makefile.in
MODULE = ofono

# data files for the module
filesdir = $(libdir)/enlightenment/modules/$(MODULE)
files_DATA = \
e-module-$(MODULE).edj module.desktop

EXTRA_DIST = $(files_DATA)

# the module .so file
INCLUDES          = -I. \
                    -I$(top_srcdir) \
                    -I$(top_srcdir)/src/modules/$(MODULE) \
                    -I$(top_srcdir)/src/bin \
                    -I$(top_builddir)/src/bin \
                    -I$(top_srcdir)/src/modules \
                    @e_cflags@ @EOFONO_CFLAGS@

pkgdir            = $(libdir)/enlightenment/modules/$(MODULE)/$(MODULE_ARCH)

pkg_LTLIBRARIES   = module.la

module_la_SOURCES = e_mod_main.h \
		    e_mod_main.c

module_la_LIBADD       = @e_libs@ @dlopen_libs@ @EOFONO_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enlightenment/modules/$(MODULE)
