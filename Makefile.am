## Makefile.am
##
## (C) Copyright 2017-2025 Fred Gleason <fredg@paravelsystems.com>
##
##   All Rights Reserved.
##
## Use automake to process this into a Makefile.in

SUBDIRS = conf\
          debian\
          helpers\
          icons\
          src\
          systemd\
          x11vnc

deb:	dist
	./build_debs.sh

rpm:	dist
	mkdir -p $(HOME)/rpmbuild/SOURCES
	cp x11vnc-auto-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target noarch-$(VENDOR)-linux x11vnc-auto.spec
	mv $(HOME)/rpmbuild/RPMS/noarch/x11vnc-auto-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/x11vnc-auto-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/x11vnc-auto-$(VERSION).tar.gz

EXTRA_DIST = build_debs.sh.in\
             get_distro.pl\
             get_target.sh\
             x11vnc-auto.spec.in\
             PACKAGE_VERSION

CLEANFILES = *~\
             *.buildinfo\
             *.changes\
             *.ddeb\
             *.deb\
             *.dll\
             *.dsc\
             *.exe\
             *.mo\
             *.rpm\
             x11vnc-auto*.tar.gz\
             *.tar.xz\
             *.tgz\
             *.zip

DISTCLEANFILES = build_debs.sh\
                 config.guess\
                 config.sub\
                 glassplayer.spec\
                 ltmain.sh

MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       compile\
                       configure\
                       depcomp\
                       install-sh\
                       Makefile.in\
                       missing
