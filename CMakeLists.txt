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

cmake_minimum_required(VERSION 3.22)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep VERSION in sync with APP_VER in src/defs.h; it also names the source
# tarball produced by CPack.
project(sfrename VERSION 1.3.1 LANGUAGES C)

# C23 with GNU extensions (equivalent to the old configure -std=gnu23 probe).
# Do not pass -std= in CMAKE_C_FLAGS, it would override this.
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if((CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 14) OR
   (CMAKE_C_COMPILER_ID MATCHES "Clang" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 18))
    message(FATAL_ERROR "a C23-capable compiler is required (GCC >= 14 or Clang >= 18)")
endif()

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
    src/string_functions.c
    src/dialogs.c
    src/images.c
)

target_link_libraries(sfrename PRIVATE PkgConfig::GTK)
target_compile_definitions(sfrename PRIVATE G_DISABLE_DEPRECATED)

include(GNUInstallDirs)
install(TARGETS sfrename RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES data/sfrename.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES data/sfrename.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)

# "cmake --build build --target uninstall" removes what "cmake --install" put
# in place, using the install_manifest.txt written by the install step.
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        @ONLY)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(test_string_functions
        tests/test_string_functions.c
        src/string_functions.c
    )
    target_include_directories(test_string_functions PRIVATE src)
    target_link_libraries(test_string_functions PRIVATE PkgConfig::GTK)
    add_test(NAME string_functions COMMAND test_string_functions)

    add_executable(test_namefn
        tests/test_namefn.c
        src/namefn.c
        src/string_functions.c
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
        src/string_functions.c
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
        src/string_functions.c
        src/rdelete.c
        src/rinsovr.c
        src/rreplace.c
        src/rnumber.c
    )
    target_include_directories(test_rconfig PRIVATE src)
    target_link_libraries(test_rconfig PRIVATE PkgConfig::GTK)
    add_test(NAME rconfig COMMAND test_rconfig)
endif()

# Packaging with CPack, run from the build directory:
#   cpack                                    binary packages (TGZ, DEB, RPM)
#   cpack -G DEB                             one generator only
#   cpack --config CPackSourceConfig.cmake   source tarballs (old "make dist")

# Binary packages. DEB and RPM only make sense on Linux; RPM also needs
# rpmbuild. DEB is written by CPack itself, so it builds on any Linux host.
set(CPACK_GENERATOR "TGZ")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND CPACK_GENERATOR "DEB")
    find_program(RPMBUILD_EXECUTABLE rpmbuild)
    if(RPMBUILD_EXECUTABLE)
        list(APPEND CPACK_GENERATOR "RPM")
    else()
        message(STATUS "rpmbuild not found, RPM package generation disabled")
    endif()
endif()

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.nongnu.org/small-file-renamer/")
set(CPACK_PACKAGE_DESCRIPTION
"Small File Renamer is a GTK+3 application for renaming files and
directories in batch: deleting, inserting, overwriting and replacing text,
numbering, changing case and converting spaces to underscores. It works
standalone, with files passed on the command line, or from Midnight
Commander's user menu.")
set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_STRIP_FILES ON)

# DEB
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Michal Babik <michal.babik@protonmail.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
# dpkg-shlibdeps computes exact library dependencies; without it (e.g. when
# building on a non-Debian host) fall back to a hand-written list.
find_program(DPKG_SHLIBDEPS_EXECUTABLE dpkg-shlibdeps)
if(DPKG_SHLIBDEPS_EXECUTABLE)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
else()
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtk-3-0 (>= 3.22), libglib2.0-0")
endif()
# CPack falls back to "i386" when dpkg is missing, so map the common
# architectures by hand.
find_program(DPKG_EXECUTABLE dpkg)
if(NOT DPKG_EXECUTABLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^i[3-6]86$")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    else()
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

# RPM (library dependencies are detected automatically by rpmbuild)
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_RELEASE_DIST ON)
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RPM_PACKAGE_GROUP "Applications/File")
set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
# Directories owned by the system "filesystem" package, not by sfrename.
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
    /usr/share/applications
    /usr/share/pixmaps)

# Source package generation.
set(CPACK_SOURCE_GENERATOR "TXZ;TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\.git/"
    "/\\.github/"
    "/build/"
    "/\\.vscode/"
    "/\\.cache/"
    "~$"
    "\\.o$"
    "compile_commands\\.json"
)
set(CPACK_PACKAGE_VENDOR "Michal Babik")
set(CPACK_PACKAGE_CONTACT "michal.babik@protonmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Batch file and directory renamer for GTK+3")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
include(CPack)
