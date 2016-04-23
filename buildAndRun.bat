@echo off

gcc  src\main.c     ^
     src\mpc\mpc.c  ^
    -std=c99        ^
    -o bin\perl.exe ^
    -Wall           ^
    -Wextra         ^
    -Werror         ^
    -fmax-errors=5

if NOT ERRORLEVEL 1 (
    echo.
    .\bin\perl.exe
)
