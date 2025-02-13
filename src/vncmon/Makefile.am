## Makefile.am
##
## Makefile for the vncmon(1) applet
##
## (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/src -Wno-strict-aliasing -std=c++17 -fPIC @QT6_CFLAGS@
LIBS = -L$(top_srcdir)/src/drouter
MOC = @QT6_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


bin_PROGRAMS = vncmon

dist_vncmon_SOURCES = cmdswitch.cpp cmdswitch.h\
                      connectionlistmodel.cpp connectionlistmodel.h\
                      connectionlistview.cpp connectionlistview.h\
                      profile.cpp profile.h\
                      vncmon.cpp vncmon.h

nodist_vncmon_SOURCES = moc_connectionlistmodel.cpp\
                        moc_connectionlistview.cpp\
                        moc_vncmon.cpp

vncmon_LDADD = @QT6_LIBS@
#vncmon_LDFLAGS = -rdynamic

EXTRA_DIST = paths.h.in

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

DISTCLEANFILES = paths.h

MAINTAINERCLEANFILES = *~\
                       Makefile.in
