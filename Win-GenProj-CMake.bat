@echo off
REM Generate CMake project files for Windows

echo ========================================
echo Wuya CMake Project Generator (Windows)
echo ========================================
echo.

REM Check if build directory exists
if not exist "build" mkdir build

REM Function to detect installed Visual Studio
set DETECTED_VS=

REM Try using vswhere (recommended method)
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist %VSWHERE% (
    for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -property displayName`) do set VS_NAME=%%i
    for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -property installationPath`) do set VS_PATH=%%i
    
    REM Detect VS version from path
    echo %VS_PATH% | find "2026" >nul 2>&1
    if %errorlevel% equ 0 set DETECTED_VS=vs2026
    
    echo %VS_PATH% | find "2022" >nul 2>&1
    if %errorlevel% equ 0 set DETECTED_VS=vs2022
    
    echo %VS_PATH% | find "2019" >nul 2>&1
    if %errorlevel% equ 0 set DETECTED_VS=vs2019
    
    echo %VS_PATH% | find "2017" >nul 2>&1
    if %errorlevel% equ 0 set DETECTED_VS=vs2017
    
    if defined VS_NAME echo Detected: %VS_NAME%
)

REM Fallback: Check registry for VS installations
if not defined DETECTED_VS (
    REM Check Visual Studio 2026
    reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\18.0" /v InstallDir >nul 2>&1
    if %errorlevel% equ 0 set DETECTED_VS=vs2026
    
    REM Check Visual Studio 2022
    if not defined DETECTED_VS (
        reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\17.0" /v InstallDir >nul 2>&1
        if %errorlevel% equ 0 set DETECTED_VS=vs2022
    )
    
    REM Check Visual Studio 2019
    if not defined DETECTED_VS (
        reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\16.0" /v InstallDir >nul 2>&1
        if %errorlevel% equ 0 set DETECTED_VS=vs2019
    )
    
    REM Check Visual Studio 2017
    if not defined DETECTED_VS (
        reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\15.0" /v InstallDir >nul 2>&1
        if %errorlevel% equ 0 set DETECTED_VS=vs2017
    )
)

REM Default generator
set GENERATOR=

REM Parse arguments (manual override)
:parse_args
if "%1"=="" goto end_parse
if /i "%1"=="vs2026" set GENERATOR="Visual Studio 18 2026"
if /i "%1"=="vs2022" set GENERATOR="Visual Studio 17 2022"
if /i "%1"=="vs2019" set GENERATOR="Visual Studio 16 2019"
if /i "%1"=="vs2017" set GENERATOR="Visual Studio 15 2017"
if /i "%1"=="ninja" set GENERATOR="Ninja"
shift
goto parse_args
:end_parse

REM If no manual override, use detected VS
if not defined GENERATOR (
    if defined DETECTED_VS (
        if "%DETECTED_VS%"=="vs2026" set GENERATOR="Visual Studio 18 2026"
        if "%DETECTED_VS%"=="vs2022" set GENERATOR="Visual Studio 17 2022"
        if "%DETECTED_VS%"=="vs2019" set GENERATOR="Visual Studio 16 2019"
        if "%DETECTED_VS%"=="vs2017" set GENERATOR="Visual Studio 15 2017"
        echo Auto-detected Visual Studio: %DETECTED_VS%
    ) else (
        echo.
        echo WARNING: No Visual Studio installation detected!
        echo.
        echo Options:
        echo   1. Install Visual Studio 2017 or later
        echo   2. Use Ninja build system: %~nx0 ninja
        echo   3. Manually specify VS version: %~nx0 vs2022
        echo.
        set GENERATOR="Ninja"
        echo Falling back to Ninja generator...
    )
)

echo.
echo Using generator: %GENERATOR%
echo.

REM Configure CMake
cd build
cmake -G %GENERATOR% -A x64 ..

if %ERRORLEVEL% neq 0 (
    echo.
    echo ========================================
    echo CMake configuration failed!
    echo ========================================
    cd ..
    exit /b 1
)

echo.
echo ========================================
echo CMake project files generated successfully!
echo Build directory: build
echo ========================================
echo.
echo Next steps:
echo   1. Open build\Wuya.sln (if using Visual Studio)
echo   2. Build the solution in your IDE
echo   Or use: cmake --build build --config Debug
echo.

cd ..
