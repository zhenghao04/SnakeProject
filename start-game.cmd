@echo off
setlocal

set "ROOT=%~dp0"
set "SOLUTION=%ROOT%MSVS\snake1.sln"
set "OUTPUT=%ROOT%MSVS\Debug\Output"
set "EXE=%OUTPUT%\snake!.exe"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "MSBUILD="

if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
        if not defined MSBUILD set "MSBUILD=%%I"
    )
)

if not defined MSBUILD (
    for /f "delims=" %%I in ('where msbuild 2^>nul') do (
        if not defined MSBUILD set "MSBUILD=%%I"
    )
)

if defined MSBUILD (
    echo [SnakeProject] Building Debug x86 version...
    "%MSBUILD%" "%SOLUTION%" /m /t:Build /p:Configuration=Debug /p:Platform=x86 /nologo
    if errorlevel 1 goto :build_failed
) else (
    if not exist "%EXE%" goto :missing_msbuild
    echo [SnakeProject] MSBuild not found. Using the existing executable.
)

if not exist "%OUTPUT%" mkdir "%OUTPUT%"
if not exist "%OUTPUT%\assets" mkdir "%OUTPUT%\assets"

copy /Y "%ROOT%external\lib\x86\*.dll" "%OUTPUT%\" >nul
if errorlevel 1 goto :deploy_failed

xcopy /E /I /Y "%ROOT%MSVS\assets" "%OUTPUT%\assets" >nul
if errorlevel 1 goto :deploy_failed

if /I "%~1"=="--build-only" goto :ready

echo [SnakeProject] Launching game...
pushd "%OUTPUT%"
start "" "%EXE%"
popd
exit /b 0

:ready
echo [SnakeProject] Build and runtime files are ready.
exit /b 0

:missing_msbuild
echo [SnakeProject] MSBuild and the compiled game were not found.
echo Install Visual Studio with the Desktop development with C++ workload, then try again.
pause
exit /b 1

:build_failed
echo [SnakeProject] Build failed.
pause
exit /b 1

:deploy_failed
echo [SnakeProject] Failed to copy runtime files.
pause
exit /b 1
