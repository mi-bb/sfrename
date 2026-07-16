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

AM_CPPFLAGS = -I$(top_srcdir)/src
AM_CFLAGS   = $(GTK_CFLAGS)
LDADD       = $(GTK_LIBS)

check_PROGRAMS = test_strfn test_namefn test_rsettings test_rconfig

test_strfn_SOURCES = test_strfn.c \
                     ../src/strfn.c

test_namefn_SOURCES = test_namefn.c \
                       ../src/namefn.c \
                       ../src/strfn.c \
                       ../src/rdelete.c \
                       ../src/rinsovr.c \
                       ../src/rreplace.c \
                       ../src/rnumber.c

test_rsettings_SOURCES = test_rsettings.c \
                          ../src/strfn.c \
                          ../src/rdelete.c \
                          ../src/rinsovr.c \
                          ../src/rreplace.c \
                          ../src/rnumber.c

test_rconfig_SOURCES = test_rconfig.c \
                        ../src/rconfig.c \
                        ../src/strfn.c \
                        ../src/rdelete.c \
                        ../src/rinsovr.c \
                        ../src/rreplace.c \
                        ../src/rnumber.c

TESTS = $(check_PROGRAMS)
