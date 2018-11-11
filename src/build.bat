@echo off

set weebp_artifacts=(wp.exe wp-headless.exe cli.obj weebp.dll weebp.lib ^
    weebp.obj)
set "weebp_libs=USER32.LIB SHELL32.LIB"

echo [ cleaning ]
for %%i in %weebp_artifacts% do echo %%i && del %%i >nul 2>&1
echo.

echo [ cli ]
call:build cli.c %* -Fewp.exe %weebp_libs% || EXIT /B 1
echo.

echo [ headless ]
call:build cli.c %* -Fewp-headless.exe %weebp_libs% ^
    -link -SUBSYSTEM:WINDOWS ^
    /entry:mainCRTStartup ^
    || EXIT /B 1
echo.

echo [ shared library ]
call:build -LD weebp.c -DWP_LIB %* -Feweebp.dll %weebp_libs% || EXIT /B 1
echo.

echo [ static library ]
call:build -c weebp.c || EXIT /B 1
lib weebp.obj || EXIT /B 1
echo.

goto:eof

:build
cl  -D_CRT_SECURE_NO_WARNINGS=1 ^
    -DNOMINMAX=1 ^
    -DWP_BUILDING ^
    -O2 -nologo -MT -Gm- -GR- -EHsc -W4 ^
    %* ^
    || EXIT /B 1
goto:eof
