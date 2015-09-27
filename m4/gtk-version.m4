# gtk-version.m4                                         -*-Autoconf-*-
# serial 1.0

dnl gEDA Prebuild checks for Gtk Library Version
dnl
dnl Copyright (C) 2015  Wiley Edward Hill <wileyhill@gmail.com>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Boston, MA 02110-1301 USA
dnl
dnl ---------------------------------------------------------------------------
dnl GTK library version
dnl ---------------------------------------------------------------------------
AC_DEFUN([AX_GTK_VERSION],
[
  AC_PREREQ([2.60])dnl

  AC_MSG_CHECKING([which gtk+ version to compile against])
  AC_ARG_WITH([gtk],
    [AS_HELP_STRING([--with-gtk=2.0|3.0],[which gtk+ version to compile against (default: 3.0)])],
    [
        case "$with_gtk" in
            2.0|3.0) ;;
            *) AC_MSG_ERROR([invalid gtk version specified]) ;;
        esac
    ],
    [with_gtk=3.0])
  AC_MSG_RESULT([$with_gtk])

  case "$with_gtk" in
  2.0) GTK_API_VERSION=2.0
       GTK_REQUIRED="$gtk2_minver"

       ;;
  3.0) GTK_API_VERSION=3.0
       GTK_REQUIRED="$gtk3_minver"
       ;;
  esac
  AC_SUBST([GTK_API_VERSION])

  []dnl
])dnl AX_DATA_DIRS
