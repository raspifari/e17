MAINTAINERCLEANFILES = Makefile.in default_tree.edj

EXTRA_DIST = \
*.edc \
../default/*.edc \
../images/* \
../images/netsync/* \
../default_elementary/*.edc \
../default_elementary/images/* \
fonts/*.ttf

EDJE_CC = @edje_cc@
EDJ = default_tree.edj

default_tree.edj:  *.edc \
../default/*.edc \
../images/* \
../images/netsync/* \
../default_elementary/*.edc \
../default_elementary/images/* 
	$(EDJE_CC) default.edc -id ../images/ -id ../default_elementary/  default_tree.edj

icondir = $(pkgdatadir)/themes
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/themes

