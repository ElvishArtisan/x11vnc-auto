## Makefile.am
##
## Makefile.am for x11vnc/
##
##   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## Use automake to process this into a Makefile.in

all:
	./build_x11vnc.sh

install-exec-local:	
	mkdir -p $(DESTDIR)$(bindir)
	cp x11vnc $(DESTDIR)$(bindir)/

uninstall-local:	
	rm -f $(DESTDIR)$(bindir)/x11vnc

EXTRA_DIST = build_x11vnc.sh.in\
             x11vnc-0.9.16ea1.tar.gz

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*\
             x11vnc

DISTCLEANFILES = build_x11vnc.sh

MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
