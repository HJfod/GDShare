@echo off
cls

chcp 65001

setlocal
call :setESC

set PROJECT_NAME=GDShare
set COMPILE_RUNNER=0
set CONFIG=RelWithDebInfo

if not exist submodules\ (
    git submodule update
)

if "%1"=="run" (
    cd build
    goto run
)

if "%1"=="cr" (
    set COMPILE_RUNNER=1
)

if "%1"=="re" (
    if exist build\ (
        rmdir /s /q build
    )

    set COMPILE_RUNNER=1
    
    mkdir build
) else (
    if not exist build\ (
        mkdir build
    )
)

cd build

echo %ESC%[93m • Generating project...%ESC%[0m

del %CONFIG%\%PROJECT_NAME%.dll

if %COMPILE_RUNNER%==1 (
    del %CONFIG%\OneTimeRunner.exe

    cmake .. -A Win32 -Thost=x86 -DCOMPILE_RUNNER=True
) else (
    cmake .. -A Win32 -Thost=x86
)

echo.
echo %ESC%[92m • Compiling library...%ESC%[0m

msbuild %PROJECT_NAME%.sln /p:Configuration=%CONFIG% /verbosity:minimal /p:PlatformTarget=x86

if not exist %CONFIG%\%PROJECT_NAME%.dll (
    rem fuck you
    echo %ESC%[91m • somwin went fuwcy wuwcy.... so sowwy.qwq... wil fix soon promis ^>w^<%ESC%[0m
    goto done
)

robocopy ..\resources %CONFIG%\resources /MIR

:run

if exist %CONFIG%\OneTimeRunner.exe (
    echo ✔️  Runner compiled
) else (
    rem fuck you
    echo %ESC%[91m • Oh nyo?!?1 i cant compiwe the x3 wunnyew.... so sowwy *huggles tightly* ^>w^<%ESC%[0m
    goto done
)

echo.
echo %ESC%[94m • Running...%ESC%[0m

cd %CONFIG%

OneTimeRunner.exe

goto done



:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
    set ESC=%%b
    exit /B 0
)
exit /B 0

:done