dnl configure.ac
dnl
dnl Autoconf configuration
dnl
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2017-2025 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl   All Rights Reserved.
dnl

AC_INIT(x11vnc-auto,m4_esyscmd(cat PACKAGE_VERSION))
AM_INIT_AUTOMAKE
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)

#
# Check for Qt6 (Mandatory)
#
AQ_FIND_QT6(QT6,Core Gui Widgets Network,[],[AC_MSG_ERROR([*** Qt6 not found ***])])

#
# Configure RPM Build
#
AC_CHECK_PROG(RPMBUILD_FOUND,rpmbuild,[yes],[])
if test -z $RPMBUILD_FOUND ; then
  AC_SUBST(RPMBUILD,rpm)
else
  AC_SUBST(RPMBUILD,rpmbuild)
fi
if test -d /usr/src/redhat ; then
  AC_SUBST(RPM_ROOT,/usr/src/redhat)
  AC_SUBST(VENDOR,redhat)
else
  AC_SUBST(RPM_ROOT,/usr/src/packages)
  AC_SUBST(VENDOR,suse)
fi 

#
# Generated File Warnings
#
AC_SUBST_FILE(GENERATED_C_FILE_WARNING)
GENERATED_C_FILE_WARNING=$srcdir/helpers/c_file_warning

AC_SUBST_FILE(GENERATED_SCRIPT_FILE_WARNING)
GENERATED_SCRIPT_FILE_WARNING=$srcdir/helpers/script_file_warning

AC_SUBST_FILE(GENERATED_XML_FILE_WARNING)
GENERATED_XML_FILE_WARNING=$srcdir/helpers/xml_file_warning

AC_CONFIG_FILES([ \
    conf/xfce4-genmon.sh \
    conf/Makefile \
    debian/Makefile \
    debian/source/Makefile \
    helpers/Makefile \
    icons/Makefile \
    src/vncmon/paths.h \
    src/vncmon/Makefile \
    src/Makefile \
    systemd/x11vnc-auto@.service \
    systemd/Makefile \
    x11vnc/build_x11vnc.sh \
    x11vnc/Makefile \
    build_debs.sh \
    x11vnc-auto.spec \
    Makefile \
])
AC_OUTPUT()
chmod 755 build_debs.sh
chmod 755 x11vnc/build_x11vnc.sh
chmod 755 conf/xfce4-genmon.sh

AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
