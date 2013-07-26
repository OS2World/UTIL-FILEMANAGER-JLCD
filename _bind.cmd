@echo off
SETLOCAL
SET PATH=G:\C600;G:\C600\BINB;G:\C600\BINP;G:\TOOLKT13\BIN;%PATH%
SET LIB=G:\TOOLKT13\LIB;G:\C600\LIB;

if not "%1"=="" goto CONT
bind
goto END

:CONT
@echo on
bind %1 api.lib doscalls.lib %2 %3 %4 %5 %6 %7 %8 %9
@echo off
rem bind %1 g:\toolkt13\lib\api.lib g:\toolkt13\lib\os2.lib %2 %3 %4 %5 %6 %7 %8 %9

:END
ENDLOCAL
