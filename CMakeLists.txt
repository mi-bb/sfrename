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

cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(sfrename VERSION 1.2.11 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
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
    src/rconfig.c
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

include(CTest)
if(BUILD_TESTING)
    add_executable(test_strfn
        tests/test_strfn.c
        src/strfn.c
    )
    target_include_directories(test_strfn PRIVATE src)
    target_link_libraries(test_strfn PRIVATE PkgConfig::GTK)
    add_test(NAME strfn COMMAND test_strfn)

    add_executable(test_namefn
        tests/test_namefn.c
        src/namefn.c
        src/strfn.c
        src/rdelete.c
        src/rinsovr.c
        src/rreplace.c
        src/rnumber.c
    )
    target_include_directories(test_namefn PRIVATE src)
    target_link_libraries(test_namefn PRIVATE PkgConfig::GTK)
    add_test(NAME namefn COMMAND test_namefn)

    add_executable(test_rsettings
        tests/test_rsettings.c
        src/strfn.c
        src/rdelete.c
        src/rinsovr.c
        src/rreplace.c
        src/rnumber.c
    )
    target_include_directories(test_rsettings PRIVATE src)
    target_link_libraries(test_rsettings PRIVATE PkgConfig::GTK)
    add_test(NAME rsettings COMMAND test_rsettings)

    add_executable(test_rconfig
        tests/test_rconfig.c
        src/rconfig.c
        src/strfn.c
        src/rdelete.c
        src/rinsovr.c
        src/rreplace.c
        src/rnumber.c
    )
    target_include_directories(test_rconfig PRIVATE src)
    target_link_libraries(test_rconfig PRIVATE PkgConfig::GTK)
    add_test(NAME rconfig COMMAND test_rconfig)
endif()
