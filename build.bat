cmake . -G "MinGW Makefiles"
mingw32-make
move FractalExplore.exe bin\FractalExplore.exe
del CMakeCache.txt /s /f /q
del cmake_install.cmake /s /f /q
del Makefile /s /f /q
@RD /S /Q ".\CMakeFiles"