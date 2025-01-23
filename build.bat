@echo off

cmake -B build -G "MinGW Makefiles"
cmake --build build --target all -j1

echo:
pause