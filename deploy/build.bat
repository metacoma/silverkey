@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

cd ../src

%QTDIR%\bin\qmake.exe CONFIG+=release
nmake
nmake install
%QTDIR%\bin\windeployqt.exe --no-translations ../bin/silverkey-qt.exe

cd ../deploy