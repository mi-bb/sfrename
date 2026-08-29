#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Copyright (C) 2019-2026 Michal Babik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

AC_PREREQ([2.69])
AC_INIT([sfrename], [1.2.10], [michal.babik@protonmail.com], [sfrename],\
        [https://www.nongnu.org/small-file-renamer/])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/sfrename.c])
AC_CONFIG_HEADERS([config.h])
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(GTK, [gtk+-3.0 >= [3.22.0]])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stddef.h stdint.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset strrchr strstr])

AC_CONFIG_FILES([Makefile src/Makefile data/Makefile tests/Makefile])
AC_OUTPUT

