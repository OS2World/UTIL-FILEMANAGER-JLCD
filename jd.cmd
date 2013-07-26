@echo off
:: Companion batch file for JLCD to be used in
:: OS/2 and Windows NT.
::
:: Public domain by:
::   Jari Laaksonen
::   Arkkitehdinkatu 30 A 2
::   FIN-33720 Tampere
::   FINLAND
::
::   Fidonet : 2:220/855.20
::   Internet: jla@to.icl.fi

if "%TMP%" == "" set TMP=d:\temp
jlcd %1 %2 > %TMP%\jlcd.cmd
if errorlevel 1 goto end
call %TMP%\jlcd.cmd

:end
del %TMP%\jlcd.cmd
