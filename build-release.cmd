@echo off
setlocal

set "ROOT=%~dp0"
set "SOLUTION=%ROOT%MSVS\snake1.sln"
set "CONFIG=Release"
set "PLATFORM=x86"
set "BUILD_OUTPUT=%ROOT%MSVS\Release\Output"
set "DIST_ROOT=%ROOT%dist"
set "PACKAGE=%DIST_ROOT%\SnakeProject"
set "ZIP=%DIST_ROOT%\SnakeProject.zip"
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

if not defined MSBUILD goto :missing_msbuild

echo [SnakeProject] Building %CONFIG% %PLATFORM% version...
"%MSBUILD%" "%SOLUTION%" /m /t:Build /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /nologo
if errorlevel 1 goto :failed

echo [SnakeProject] Creating release package...
if exist "%PACKAGE%" rmdir /S /Q "%PACKAGE%"
if not exist "%DIST_ROOT%" mkdir "%DIST_ROOT%"
mkdir "%PACKAGE%"
mkdir "%PACKAGE%\assets"
mkdir "%PACKAGE%\docs"
mkdir "%PACKAGE%\user"

copy /Y "%BUILD_OUTPUT%\snake!.exe" "%PACKAGE%\" >nul
if errorlevel 1 goto :failed

copy /Y "%ROOT%external\lib\x86\*.dll" "%PACKAGE%\" >nul
if errorlevel 1 goto :failed

xcopy /E /I /Y "%ROOT%MSVS\assets" "%PACKAGE%\assets" >nul
if errorlevel 1 goto :failed

xcopy /E /I /Y "%ROOT%docs" "%PACKAGE%\docs" >nul
if errorlevel 1 goto :failed

copy /Y "%ROOT%README.md" "%PACKAGE%\" >nul
copy /Y "%ROOT%LICENSE" "%PACKAGE%\" >nul
copy /Y "%ROOT%start-game.cmd" "%PACKAGE%\" >nul

if /I "%~1"=="--no-zip" goto :ready

if exist "%ZIP%" del /Q "%ZIP%"
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%PACKAGE%\*' -DestinationPath '%ZIP%' -Force"
if errorlevel 1 goto :failed

echo [SnakeProject] Zip package: %ZIP%

:ready
echo [SnakeProject] Release package: %PACKAGE%
exit /b 0

:missing_msbuild
echo [SnakeProject] MSBuild was not found.
echo Install Visual Studio with the Desktop development with C++ workload, then try again.
exit /b 1

:failed
echo [SnakeProject] Release build failed.
exit /b 1
