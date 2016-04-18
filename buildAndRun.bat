@echo off

gcc -std=c99 src\main.c -o bin\perl.exe -Wall -Wextra -Werror -fmax-errors=5

if NOT ERRORLEVEL 1 (
    echo.
    .\bin\perl.exe
)
