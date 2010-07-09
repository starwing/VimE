@echo off
:: build VimE automate.
:: see README for details.

set __CMAKEFLAGS=

::if "%1x" == "x" goto :usage
if "%1x" == "x" goto :minSYS
if "%1" == "minGW" goto :minGW
if "%1" == "minSYS" goto :minSYS
if "%1" == "VC" goto :VC

:usage
echo use specified tool to build VimE.
echo [usage] %0 tool
echo current supported tool: minGW, VC
echo.
goto :exit


:minGW
set MAKE=mingw32-make
set __CMAKEFLAGS=-G "MinGW Makefiles" %CMAKEFLAGS%
goto :build

:minSYS
set MAKE=make
set __CMAKEFLAGS=-G "MSYS Makefiles" %CMAKEFLAGS%
goto :build


:build
if not defined CMAKE set CMAKE=cmake
if not defined MAKE set MAKE=make
if not defined MAKEFLAGS set MAKEFLAGS=

pushd %~dp0build
if "%2" == "force" if exist CMakeCache.txt del CMakeCache.txt
%CMAKE% %__CMAKEFLAGS% ..
if not %ERRORLEVEL% == 0 goto :error

%MAKE% %MAKEFLAGS%

goto :exit


:error
popd
goto :exit

:exit
@echo on

