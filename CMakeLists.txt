# Copyright (C) 2019-2020 Michal Babik
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

cmake_minimum_required(VERSION 3.10)
project(sfrename VERSION 1.2.10 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0>=3.22)

add_executable(sfrename
    src/sfrename.c
    src/rdelete.c
    src/rinsovr.c
    src/rreplace.c
    src/rnumber.c
    src/rendata.c
    src/rfnames.c
    src/rfitem.c
    src/namefn.c
    src/strfn.c
    src/dlgs.c
    src/imgs.c
)

target_link_libraries(sfrename PRIVATE PkgConfig::GTK)
target_compile_definitions(sfrename PRIVATE G_DISABLE_DEPRECATED)

include(GNUInstallDirs)
install(TARGETS sfrename RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES data/sfrename.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES data/sfrename.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
