dnl acinclude.m4
dnl
dnl Local Autoconf macro definitions
dnl
dnl   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License version 2 as
dnl   published by the Free Software Foundation.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public
dnl   License along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl

dnl AR_GCC_TARGET()
dnl
dnl Get gcc(1)'s idea of the target architecture, distribution and os.
dnl The following variables are set:
dnl   $ar_gcc_arch = Target Architecture (i586, XF86_64, etc)
dnl   $ar_gcc_distro = Target Distribution (suse, slackware, etc)
dnl   $ar_gcc_os = Target Operating System (linux, solaris, etc)
dnl
AC_DEFUN([AR_GCC_TARGET],[AC_REQUIRE([AC_PROG_CC])]
  [
  AC_MSG_CHECKING(target architecture)
  ar_gcc_arch=$(./get_target.sh $CC $AWK arch)
  ar_gcc_distro=$(./get_target.sh $CC $AWK distro)
  ar_gcc_os=$(./get_target.sh $CC $AWK os)
  AC_MSG_RESULT([$ar_gcc_arch-$ar_gcc_distro-$ar_gcc_os])
  ]
)

dnl AR_GET_DISTRO()
dnl
dnl Try to determine the name and version of the distribution running
dnl on the host machine, based on entries in '/etc/'.
dnl The following variables are set:
dnl   $ar_distro_name = Distribution Name (SuSE, Debian, etc)
dnl   $ar_distro_version = Distribution Version (10.3, 3.1, etc)
dnl
AC_DEFUN([AR_GET_DISTRO],[]
  [
  AC_MSG_CHECKING(distribution)
  ar_distro_name=$(./get_distro.pl NAME $AWK)
  ar_distro_version=$(./get_distro.pl VERSION $AWK)
  AC_MSG_RESULT([$ar_distro_name $ar_distro_version])
  ]
)


dnl AQ_FIND_QT6(prefix,list-of-modules,action-if-found,action-if-not-found)
dnl
dnl Attempt to find a Qt6 installation.
dnl
dnl By default, this will look for a system-wide install, at
dnl '/usr/lib/<machine>/qt6/qt6.conf', with <machine> being gcc(1)'s
dnl idea of the machine's arch/os/distro --e.g. 'x86_64-linux-gnu'. This can
dnl overridden by defining a 'QT6_PATH' environmental variable containing
dnl the path to the configuration file of the desired Qt6 installation.
dnl
dnl If an installation is successfully detected, the following variables
dnl are set:
dnl
dnl prefix_CFLAGS - Flags to pass to the C++ compiler
dnl
dnl prefix_LIBS - Linker arguments
dnl
dnl prefix_MOC - Path to the appropriate moc(1)
dnl
AC_DEFUN([AQ_FIND_QT6],[AC_REQUIRE([AC_PROG_CXX])]
  [
  AC_MSG_CHECKING([for ]$1)

  dnl
  dnl Load the Qt6 configuration
  dnl
  AC_ARG_VAR(QT6_PATH,[path to the location of the Qt6 installation's configuration (default: "/usr/lib/<machine>/qt6/qt6.conf")])

  if test -n "$QT6_PATH" ; then
    config_path=$QT6_PATH
    if ! test -f $config_path ; then
      config_path=""
    fi
  else
    config_path=/usr/lib/$(gcc -dumpmachine)/qt6/qt6.conf
    if ! test -f $config_path ; then
      config_path=/usr/lib/$(gcc -dumpmachine)/qtchooser/qt6.conf
      if ! test -f $config_path ; then
        config_path=""
      fi
    fi
  fi
  if test $config_path ; then
      qt_var=$(grep ^Prefix\= $config_path)
      qt_Prefix=${qt_var:7}
      qt_var=$(grep ^ArchData\= $config_path)
      qt_ArchData=${qt_var:9}
      qt_var=$(grep ^Binaries\= $config_path)
      qt_Binaries=${qt_var:9}
      qt_var=$(grep ^Data\= $config_path)
      qt_Data=${qt_var:5}
      qt_var=$(grep ^Documentation\= $config_path)
      qt_Documentation=${qt_var:14}
      qt_var=$(grep ^Headers\= $config_path)
      qt_Headers=${qt_var:8}
      qt_var=$(grep ^HostBinaries\= $config_path)
      qt_HostBinaries=${qt_var:13}
      qt_var=$(grep ^HostData\= $config_path)
      qt_HostData=${qt_var:9}
      qt_var==$(grep ^HostLibraries\= $config_path)
      qt_HostLibraries=${qt_var:15}
      qt_var=$(grep ^Libraries\= $config_path)
      qt_Libraries=${qt_var:10}
      qt_var=$(grep ^LibraryExecutables\= $config_path)
      qt_LibraryExecutables=${qt_var:19}
      qt_var=$(grep ^Plugins\= $config_path)
      qt_Plugins=${qt_var:8}
      qt_var=$(grep ^Qml2Imports\= $config_path)
      qt_Qml2Imports=${qt_var:12}
      qt_var=$(grep ^Settings\= $config_path)
      qt_Settings=${qt_var:9}
      qt_var=$(grep ^Translations\= $config_path)
      qt_Translations=${qt_var:13}
      qt_libs="-L "$qt_Prefix$qt_Libraries
      qt_cppflags="-I"$qt_Prefix"/"$qt_Headers
      qt_libs="-L"$qt_Prefix"/"$qt_Libraries
      qt_moc=$qt_Prefix"/"$qt_Binaries"/moc"
      if test -n "$QT6_MOC" ; then
        qt_moc=$QT6_MOC
      fi
      if test -x $qt_moc ; then
        for module in $2
        do
        :
	  qt_cppflags=$qt_cppflags" -DQT_"${module^^}"_LIB -I"$qt_Prefix"/"$qt_Headers"/Qt"$module
	  qt_libs=$qt_libs" -lQt6"$module
        done
        AC_ARG_VAR([$1][_CFLAGS],[C++ compiler flags for $1, overriding autodetected values])
        AC_ARG_VAR([$1][_LIBS],[linker flags for $1, overriding autodetected values])
        AC_ARG_VAR([$1][_MOC],[path to the moc(1) binary, overriding autodetected values])
        AC_SUBST([$1][_CFLAGS],$qt_cppflags)
        AC_SUBST([$1][_LIBS],$qt_libs)
        AC_SUBST([$1][_MOC],$qt_moc)
        AC_MSG_RESULT([found])
        $3
      else
        AC_MSG_RESULT([moc(1) not found])
	$4
      fi
  else
      AC_MSG_RESULT([not found])
      $4
  fi
  ]
)
