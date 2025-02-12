## Makefile.am
##
## icons/Makefile.am
##
## (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
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
##
## Use automake to process this into a Makefile.in

install-exec-am:	
	mkdir -p $(DESTDIR)@prefix@/share/x11vnc-auto
	cp vncmon-0.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-1.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-2.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-3.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-4.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-5.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-6.png $(DESTDIR)@prefix@/share/x11vnc-auto/
	cp vncmon-many.png $(DESTDIR)@prefix@/share/x11vnc-auto/

uninstall-local:	
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-0.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-1.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-2.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-3.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-4.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-5.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-6.png
	rm -f $(DESTDIR)@prefix@/share/x11vnc-auto/vncmon-many.png

EXTRA_DIST = vncmon-16x16.png\
             vncmon-22x22.png\
             vncmon-32x32.png\
             vncmon-0.png\
             vncmon-1.png\
             vncmon-2.png\
             vncmon-3.png\
             vncmon-4.png\
             vncmon-5.png\
             vncmon-6.png\
             vncmon-many.png

CLEANFILES = *~

MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in
