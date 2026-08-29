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

AC_PREREQ([2.72])
AC_INIT([sfrename], [1.2.11], [michal.babik@pm.me], [sfrename],
        [https://www.nongnu.org/small-file-renamer/])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/sfrename.c])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC

# Require C23 (ISO/IEC 9899:2024) with GNU extensions.  Autoconf 2.72 does not
# know about C23 yet, so probe the compiler for the right -std option here.
AC_CACHE_CHECK([for $CC option to enable C23 features], [sfr_cv_prog_cc_c23],
  [sfr_cv_prog_cc_c23=no
   sfr_save_CC=$CC
   for sfr_std in '' -std=gnu23 -std=gnu2x -std=c23 -std=c2x; do
     CC="$sfr_save_CC $sfr_std"
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[#if __STDC_VERSION__ < 202311L
# error "C23 support is required"
#endif]], [])],
       [AS_IF([test -z "$sfr_std"],
              [sfr_cv_prog_cc_c23="none needed"],
              [sfr_cv_prog_cc_c23=$sfr_std])
        break])
   done
   CC=$sfr_save_CC])
AS_IF([test "x$sfr_cv_prog_cc_c23" = xno],
      [AC_MSG_ERROR([a C23-capable compiler is required (GCC >= 14 or Clang >= 18)])])
AS_IF([test "x$sfr_cv_prog_cc_c23" != "xnone needed"],
      [CC="$CC $sfr_cv_prog_cc_c23"])

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
