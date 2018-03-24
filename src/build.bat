@echo off

del wp.exe >nul 2>&1
del cli.obj >nul 2>&1
del weebp.dll >nul 2>&1
del weebp.lib >nul 2>&1
del weebp.obj >nul 2>&1

set "weebp_libs=USER32.LIB SHELL32.LIB"

echo *** cli
call:build cli.c %* -Fewp.exe %weebp_libs%

echo *** headless
call:build cli.c %* -Fewp-headless.exe %weebp_libs% ^
    -link -SUBSYSTEM:WINDOWS ^
    /entry:mainCRTStartup

echo *** shared library
call:build -LD weebp.c -DWP_LIB %* -Feweebp.dll %weebp_libs%

echo *** static library
link -DLL -OUT:weebp.lib weebp.obj || EXIT /B 1

goto:eof

:build
cl  -D_CRT_SECURE_NO_WARNINGS=1 ^
    -DNOMINMAX=1 ^
    -DWP_BUILDING ^
    -O2 -nologo -MT -Gm- -GR- -EHsc -W4 ^
    %* ^
    || EXIT /B 1
goto:eof