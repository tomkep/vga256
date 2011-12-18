This is BGI driver for 256 colors VGA modes:
- 320x200x256
- 320x400x256

I wrote this driver in the first half of nineties for:
- Borland C++ 2.0
- Borland C++ 3.1
- Borland Pascal 7.0.

Tools required to build the driver:
- Turbo Assembler 2.51 (newer version may work)
- Borland C++ 2.0 or 3.1
- Borland MAKE 3.5

The makefile as provided will work for Borland C++ 2.0 & 3.1.

To build version 2 BGI driver simply issue 'make' in the directory with all
files.

The version 2 BGI driver will work with Borland C++ 2.0 and 3.1 and real mode
Borland/Turbo Pascal 7.0. It probably will works with Turbo C 2.0,
Turbo C++ 1.0, Turbo Pascal 5.5 and Turbo Pascal 6.0.

To build version 3 BGI driver issue 'make -DBGI30' in the directory with all
files.

The version 3 BGI driver will work with Borland/Turbo Pascal 7.0 in both
real and 16-bit protected mode. It will not work with Borland C++ 2.0 & 3.1.
It may work with Borland C++ 4.0 and up and may also work with 16-bit
protected mode C++ applications generated with Borland C++ 4.0 and Borland
Power Pack (this is just my wild guess, I never had a chance to try it).
It will not work with 32-bit protected mode C++ applications.

All the files except device.inc are licensed under MIT license
(see license.txt).

device.inc is a part of BGI TOOLKIT VERSION 1.0. See the file for copyright
information. It might have been modified (to provide support for version 3 BGI)
and I license any changes I might have done under MIT licence.
