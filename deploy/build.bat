@echo off

cd /d %~dp0

call "%QTDIR%\bin\qtenv2.bat"
call "%VS150COMNTOOLS%..\..\VC\Auxiliary\Build\vcvars64.bat"

cd /d %~dp0
cd ../src

qmake CONFIG+=release
nmake
nmake install
windeployqt --no-translations ../bin/silverkey-qt.exe

cd /d %~dp0

