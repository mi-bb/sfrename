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
  * [Building with CMake](#building-with-cmake)
* [Running](#running)
* [Contact and help](#contact-and-help)

## About

Small File Renamer is an application for renaming files and directories.

The program runs on GNU/Linux and FreeBSD.

It was originally designed to work with Midnight Commander, but it can also
be used standalone or with other programs.

## Requirements

* GNU/Linux or FreeBSD
* GCC or Clang
* GTK+ 3 (3.22 or later)

## Installation

### Basic installation

You need Autotools installed for the `autogen.sh` script to generate the
`configure` script and Makefiles. If you obtained the standard program
package (a release tarball), these generated files are already included and
Autotools are not required.

Commands to configure, build, and install the program:

```sh
./autogen.sh
./configure
make
make install
```

Although, it's recommended to configure and compile with the more detailed
options described below.

### Compilers and options

For normal daily use of this program, a good option is:

```sh
./configure CFLAGS="-march=native -O2 -pipe"
```

or, more specifically:

```sh
./configure CC="gcc" CFLAGS="-march=native -O2 -pipe -std=gnu11"
```

with Clang:

```sh
./configure CC="clang" CFLAGS="-march=native -O2 -pipe -std=gnu11"
```

Setting `CFLAGS` explicitly overrides the default flags (`-g -O2`), which
disables the `-g` option. That option produces debugging information needed
for gdb and enlarges the output file, so leaving it out is preferable for a
release build.

* `CC="gcc"` sets the C compiler to GCC.
* `CC="clang"` sets the C compiler to Clang.
* `-march=native` enables all instruction subsets supported by the local
  machine.
* `-O2` sets the code optimization level to O2.
* `-pipe` uses pipes rather than temporary files for communication between
  the various stages of compilation.
* `-std=gnu11` sets the C standard to C11 with GNU extensions.

Running:

```sh
./configure --help
```

will print a detailed description of the available initial values for the
configuration parameters.

### Building with CMake

As an alternative to Autotools, the program can also be configured, built,
and installed with CMake (3.10 or later):

```sh
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

Compiler and optimization flags are set the same way as with any other
CMake project, for example:

```sh
cmake -S . -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-march=native -O2 -pipe"
```

The unit tests can be built and run with CTest:

```sh
ctest --test-dir build
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
