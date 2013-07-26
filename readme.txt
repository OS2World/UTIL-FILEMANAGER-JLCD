
This package contains public domain code for windowed directory change
program. The files in the package are:

JLCD.CPP      main program.
JLCD.EXE      Compiled family mode program.
VM_LB.C       VidMgr listbox.
VM_LB.H       VidMgr listbox header file.
STRISTR.C     modified version of Mark Kimes' stristr().
GETDISK.C     portable Borland style getdisk() and setdisk().
FFBUFF.CPP    portable class for FindFirst/FindNext operations.
FFBUFF.H      header file for ffbuff.cpp
VIDMGR.H      modified VidMgr 1.3 header file.
VMGRWNT.C     modified VidMgr 1.3 module for Win32.
JD.CMD        JLCD companion batch file to be used in OS/2 and WinNT.
MAKEFILE.DOS  makefile for Watcom DOS compiler.
MAKEFILE.IBM  makefile for IBM Visual Age C++.
MAKEFILE.MSC  makefile for MSVC++ DOS compiler.
MAKEFILE.WAT  makefile for Watcom OS/2 (native and family mode).
MAKEFILE.WNT  makefile for Watcom Win32 compiler.
_BIND.CMD     wrapper for BIND.EXE.
VIDMGR.C      VidMgr 1.3 module from SNIPPETS.
VMGROS2.C     VidMgr 1.3 module from SNIPPETS.
VMGRDOS.C     VidMgr 1.3 module from SNIPPETS.
OPSYS.C       VidMgr 1.3 module from SNIPPETS.
OPSYS.H       VidMgr 1.3 module from SNIPPETS.

There are plenty of change directory programs, and best for my use so far has
been Les J. Ventimiglia's LCD 6.0 for OS/2, but it choked on my 4900+
directory G: drive :-(

I have compiled JLCD with DOS, Win32 and OS/2 compilers, and even the DOS
version can now handle my G: drive!

I have NOT compiled JLCD with all compilers that are mentioned in
VidMgr documents, so let me know if it doesn't compile/run on some
platforms.

If you have Watcom and BIND.EXE from MSC 6.0 or IBM OS/2 Toolkit 1.3, you may
want to try to compile JLCD as family mode program so that you can use it in
DOS, WinNT and OS/2 computers, if you happen to have so many.

   Jari Laaksonen
   Arkkitehdinkatu 30 A 2
   FIN-33720 Tampere
   FINLAND

   Fidonet : 2:220/855.20
   Internet: jla@to.icl.fi
