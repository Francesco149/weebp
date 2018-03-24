@echo off

del wp.exe >nul 2>&1
del cli.obj >nul 2>&1
del weebp.dll >nul 2>&1
del weebp.lib >nul 2>&1
del weebp.obj >nul 2>&1

echo *** cli
cl  -D_CRT_SECURE_NO_WARNINGS=1 ^
    -DNOMINMAX=1 ^
    -DWP_BUILDING ^
    -O2 -nologo -MT -Gm- -GR- -EHsc -W4 ^
    %* ^
    cli.c ^
    -Fewp.exe ^
    USER32.LIB ^
    SHELL32.LIB ^
    || EXIT /B 1

echo *** headless
cl  -D_CRT_SECURE_NO_WARNINGS=1 ^
    -DNOMINMAX=1 ^
    -DWP_BUILDING ^
    -O2 -nologo -MT -Gm- -GR- -EHsc -W4 ^
    %* ^
    cli.c ^
    -Fewp-headless.exe ^
    USER32.LIB ^
    SHELL32.LIB ^
    -link -SUBSYSTEM:WINDOWS ^
    /entry:mainCRTStartup ^
    || EXIT /B 1

echo *** shared library
cl  -D_CRT_SECURE_NO_WARNINGS=1 ^
    -DNOMINMAX=1 ^
    -DWP_BUILDING ^
    -DWP_LIB ^
    -LD -O2 -nologo -MT -Gm- -GR- -EHsc -W4 ^
    %* ^
    weebp.c ^
    -Feweebp.dll ^
    USER32.LIB ^
    SHELL32.LIB ^
    || EXIT /B 1

echo *** static library
link -DLL -OUT:weebp.lib weebp.obj || EXIT /B 1