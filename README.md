# Small File Renamer (sfrename)

## Copyright and License

Copyright (C) 2019-2026 Michal Babik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

## Contents of this file

* [Copyright and license](#copyright-and-license)
* [About](#about)
* [Requirements](#requirements)
* [Installation](#installation)
  * [Basic installation](#basic-installation)
  * [Compilers and options](#compilers-and-options)
  * [Running the tests](#running-the-tests)
  * [Uninstalling](#uninstalling)
  * [Making a source package](#making-a-source-package)
* [Running](#running)
* [Contact and help](#contact-and-help)

## About

Small File Renamer is an application for renaming files and directories.

The program runs on GNU/Linux and FreeBSD.

It was originally designed to work with Midnight Commander, but it can also
be used standalone or with other programs.

## Requirements

* GNU/Linux or FreeBSD
* A C23-capable compiler: GCC 14 or later, or Clang 18 or later
* GTK+ 3 (3.22 or later)
* CMake 3.21 or later

## Installation

### Basic installation

The program is built with CMake. Commands to configure, build, and install
it:

```sh
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

The first command creates a `build` directory and configures the project in
it, the second compiles, and the third installs the binary, the `.desktop`
file, and the icon under `/usr/local` by default. A different location can
be chosen with `--prefix`:

```sh
cmake --install build --prefix /usr
```

Although the commands above are enough, it's recommended to configure and
compile with the more detailed options described below.

### Compilers and options

For normal daily use of this program, a good option is:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-march=native -O2 -pipe"
```

or, more specifically:

```sh
cmake -S . -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-march=native -O2 -pipe"
```

with Clang:

```sh
cmake -S . -B build -DCMAKE_C_COMPILER=clang -DCMAKE_C_FLAGS="-march=native -O2 -pipe"
```

The default `CMAKE_BUILD_TYPE` is empty, which adds no flags of its own, so
whatever is passed in `CMAKE_C_FLAGS` is what the compiler gets. `Release`
adds `-O3 -DNDEBUG`, `Debug` adds `-g`. Debugging information is needed for
gdb and enlarges the output file, so leaving it out is preferable for a
release build.

There is no need to pass `-std=` yourself: the project sets the C standard
to C23 with GNU extensions (`-std=gnu23`). Do not put a `-std=` option in
`CMAKE_C_FLAGS`, or it will override the one CMake selects.

* `-DCMAKE_C_COMPILER=gcc` sets the C compiler to GCC.
* `-DCMAKE_C_COMPILER=clang` sets the C compiler to Clang.
* `-march=native` enables all instruction subsets supported by the local
  machine.
* `-O2` sets the code optimization level to O2.
* `-pipe` uses pipes rather than temporary files for communication between
  the various stages of compilation.

Running:

```sh
cmake -S . -B build -LH
```

will print a detailed description of the available configuration variables.

### Running the tests

The unit tests are built together with the program and are run with CTest:

```sh
ctest --test-dir build
```

A single suite can be selected by name:

```sh
ctest --test-dir build -R strfn
```

### Uninstalling

From the same build directory the program was installed from:

```sh
sudo cmake --build build --target uninstall
```

This removes every file listed in the build directory's
`install_manifest.txt`.

### Making a source package

A source tarball (`.tar.xz` and `.tar.gz`) is created with CPack:

```sh
cd build
cpack --config CPackSourceConfig.cmake
```

## Running

The program can be run in standalone mode, or files can be passed as
command-line arguments.

Example of use when passing files as command-line arguments:

```sh
sfrename filename1.ext filename2.ext
```

This program was made to be used with Midnight Commander to rename files
and directories.

Example use with MC:

```
+ ! t t
R       Rename file
        sfrename %f
+ t t
R       Rename files
        sfrename %t
```

These are sample entries for the Midnight Commander menu file. You can
rename files by selecting them in mc, pressing 'F2', then choosing 'R' from
the menu; the selected files and directories will be passed to the program
as command-line arguments.

## Contact and help

If you have problems with the program, please contact me:
michal.babik@protonmail.com
