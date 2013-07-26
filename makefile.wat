#
# MAKEFILE.WAT
# Makefile for JLCD directory changer -- Watcom OS/2 compiler.
#
# This is an example how you can make Family mode programs
# using modern language (C++) and old but good tools (BIND.EXE
# and IBM's OS/2 1.3 Toolkit or MS C 6.00).
#
# Family mode programs (sometimes called as 'bound' programs)
# are programs that run in both OS/2 and DOS, including
# Windows 95 and Windows NT DOS boxes.
#
# ver 1.0, 05 Sep 1998
#
# Public domain by:
#   Jari Laaksonen
#   Arkkitehdinkatu 30 A 2
#   FIN-33720 Tampere
#   FINLAND
#
#   Fidonet : 2:220/855.20
#   Internet: jla@to.icl.fi
#

INC    = D:\WATCOM\H;G:\ToolKT13\C\INCLUDE;
CFLAGS = -w4 -zq -bt=os2 -e25
LINK   = wlink op st=0x8000 d all op m op q op symf op c op maxe=25
OBJS   = ffbuff.obj jlcd.obj stristr.obj getdisk.obj vidmgr.obj vmgros2.obj vm_lb.obj
LOBJS  = ffbuff.obj,jlcd.obj,stristr.obj,getdisk.obj,vidmgr.obj,vmgros2.obj,vm_lb.obj
PROJ   = jlcd

!ifdef 16bit
WCC    = wcc
WPP    = wpp
CFLAGS = $(CFLAGS) -ml -2 -i=$(INC)
LINK   = $(LINK) SYS os2
PROJ   = $(PROJ)16
!else
WCC    = wcc386
WPP    = wpp386
LINK   = $(LINK) SYS os2v2
!endif

!ifdef debug
CFLAGS = $(CFLAGS) -d2 -od -D_DEBUG
!else
CFLAGS = $(CFLAGS) -otmlra -s -DNDEBUG
!endif

$(PROJ).exe: $(OBJS)
  $(LINK) NAME $(PROJ) FIL $(LOBJS)
!ifdef 16bit
  _bind $(PROJ).exe
!endif

.cpp.obj:
  $(WPP) $(CFLAGS) $<

.c.obj:
  $(WCC) $(CFLAGS) $<

clean:
  del $(OBJS) $(PROJ).sym
