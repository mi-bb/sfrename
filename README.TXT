CONTENTS OF THIS FILE
----------------------

 * About
 * Copyright and license
 * Requirements
 * Instalation and running
  * Compilation
  * Example of use
  * Example of use with Midnight Commander
 * Contact and help


About
------

Small File Renamer is a program for renaming files and directories.

Program works in GNU/Linux.

It was originally made to work with Midnight Commander but it may be used
separately or with some other program.


Copyright and license
----------------------

Copyright (C) 2019 Michał Bąbik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.


Requirements
-------------

 * GNU/Linux
 * GCC (my version was 7.3.0)
 * GTK+ 3 (3.22.30)
 * GLib (2.27)

Program may work in previous versions, I just wrote on which I was compiling
this program and there were no problems.


Instalation and running
------------------------

Compile source file with gcc, example compile command is showed below:

gcc -std=c11 -Wall `pkg-config --cflags gtk+-3.0` -o sfrename sfrename.c \
`pkg-config --libs gtk+-3.0`

File names should be passed to program as arguments, example of use:

sfrename filename1.ext filename2.ext

I made this program to use it with Midnight Commander to rename files and
directories.
To use it in MC you can make a menu entry, that will run this program with
selected files and directories.

Example use with MC:

If you put lines shown below in Midnight Commander menu file, you can
rename files by selecting them, pressing 'F2' and in menu the 'R' key,
selected files and dirs will be passed to program in command line as
program arguments.

MC menu lines:

+ ! t t
R       Rename file
        ~/sfrename %f
+ t t
R       Rename files
        ~/sfrename %t


Contact and help
-----------------

In case of problems with the program, please contact me:
michalb1981@o2.pl

