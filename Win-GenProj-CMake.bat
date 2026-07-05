@echo off
REM Generate CMake project files for Windows

REM Enable delayed expansion for safe variable expansion inside blocks
setlocal enabledelayedexpansion

echo ========================================
echo Wuya CMake Project Generator (Windows)
echo ========================================
echo.

REM Build configuration defaults
set BUILD_TYPE=Debug
set GENERATOR=
set FORCE_GENERATOR=0

REM Visual Studio detection results
set DETECTED_VS=
set VS_NAME=
set VS_PATH=
set VS_LINE_VER=
set VCVARSALL=
set CMAKE_EXE=cmake

REM ----------------------------------------------------------------------
REM Detect installed Visual Studio (prefer vswhere)
REM ----------------------------------------------------------------------
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist %VSWHERE% (
    for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -property displayName 2^>nul`) do set VS_NAME=%%i
    for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -property installationPath 2^>nul`) do set VS_PATH=%%i
    for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -property catalog_productLineVersion 2^>nul`) do set VS_LINE_VER=%%i

    if defined VS_NAME echo Detected: !VS_NAME!
    if defined VS_PATH echo InstallationPath: !VS_PATH!
    if defined VS_LINE_VER echo Version: !VS_LINE_VER!

    REM Map product line version to a short identifier
    if "!VS_LINE_VER!"=="18" set DETECTED_VS=vs2026
    if "!VS_LINE_VER!"=="17" set DETECTED_VS=vs2022
    if "!VS_LINE_VER!"=="16" set DETECTED_VS=vs2019
    if "!VS_LINE_VER!"=="15" set DETECTED_VS=vs2017

    REM Fallback: try to parse the display name if catalog version is empty
    if not defined DETECTED_VS (
        echo !VS_NAME! | find "2026" >nul 2>&1 && set DETECTED_VS=vs2026
        echo !VS_NAME! | find "2022" >nul 2>&1 && set DETECTED_VS=vs2022
        echo !VS_NAME! | find "2019" >nul 2>&1 && set DETECTED_VS=vs2019
        echo !VS_NAME! | find "2017" >nul 2>&1 && set DETECTED_VS=vs2017
    )
)

REM ----------------------------------------------------------------------
REM Prefer the CMake bundled with Visual Studio
REM It is the only CMake guaranteed to support the matching VS generator.
REM ----------------------------------------------------------------------
if defined VS_PATH (
    set VS_CMAKE=!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe
    if exist "!VS_CMAKE!" (
        set CMAKE_EXE="!VS_CMAKE!"
        for /f "usebackq tokens=*" %%i in (`"!VS_CMAKE!" --version 2^>nul`) do echo Using VS-bundled CMake: %%i
    )
)

if "!CMAKE_EXE!"=="cmake" (
    for /f "usebackq tokens=*" %%i in (`cmake --version 2^>nul`) do echo Using system CMake: %%i
)

REM Fallback: Check registry for VS installations
if not defined DETECTED_VS (
    reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\18.0" /v InstallDir >nul 2>&1
    if !errorlevel! equ 0 set DETECTED_VS=vs2026

    if not defined DETECTED_VS (
        reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\17.0" /v InstallDir >nul 2>&1
        if !errorlevel! equ 0 set DETECTED_VS=vs2022
    )

    if not defined DETECTED_VS (
        reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\16.0" /v InstallDir >nul 2>&1
        if !errorlevel! equ 0 set DETECTED_VS=vs2019
    )

    if not defined DETECTED_VS (
        reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\15.0" /v InstallDir >nul 2>&1
        if !errorlevel! equ 0 set DETECTED_VS=vs2017
    )
)

REM ----------------------------------------------------------------------
REM Parse command-line arguments
REM ----------------------------------------------------------------------
:parse_args
if "%1"=="" goto end_parse
if /i "%1"=="vs2026" set GENERATOR=Visual Studio 18 2026&set FORCE_GENERATOR=1
if /i "%1"=="vs2022" set GENERATOR=Visual Studio 17 2022&set FORCE_GENERATOR=1
if /i "%1"=="vs2019" set GENERATOR=Visual Studio 16 2019&set FORCE_GENERATOR=1
if /i "%1"=="vs2017" set GENERATOR=Visual Studio 15 2017&set FORCE_GENERATOR=1
if /i "%1"=="ninja"  set GENERATOR=Ninja&set FORCE_GENERATOR=1
if /i "%1"=="debug"    set BUILD_TYPE=Debug
if /i "%1"=="release"  set BUILD_TYPE=Release
if /i "%1"=="shipping" set BUILD_TYPE=Shipping
shift
goto parse_args
:end_parse

REM ----------------------------------------------------------------------
REM Determine generator if not manually specified
REM ----------------------------------------------------------------------
if not defined GENERATOR (
    if not defined DETECTED_VS (
        echo.
        echo WARNING: No Visual Studio installation detected!
        echo.
        echo Options:
        echo   1. Install Visual Studio 2017 or later
        echo   2. Use Ninja build system: %~nx0 ninja
        echo   3. Manually specify VS version: %~nx0 vs2022
        echo.
        set GENERATOR=Ninja
        echo Falling back to Ninja generator...
    ) else (
        echo Auto-detected Visual Studio: !DETECTED_VS!

        if "!DETECTED_VS!"=="vs2026" set GENERATOR=Visual Studio 18 2026
        if "!DETECTED_VS!"=="vs2022" set GENERATOR=Visual Studio 17 2022
        if "!DETECTED_VS!"=="vs2019" set GENERATOR=Visual Studio 16 2019
        if "!DETECTED_VS!"=="vs2017" set GENERATOR=Visual Studio 15 2017
    )
)

if not defined GENERATOR (
    echo.
    echo ERROR: Could not determine a suitable CMake generator.
    exit /b 1
)

REM ----------------------------------------------------------------------
REM Validate that the selected CMake actually supports the chosen generator
REM ----------------------------------------------------------------------
set GENERATOR_SUPPORTED=
set CMAKE_HELP_FILE=%TEMP%\WuyaCMakeHelp_%RANDOM%.txt
!CMAKE_EXE! --help > "!CMAKE_HELP_FILE!" 2>nul
findstr /C:"!GENERATOR!" "!CMAKE_HELP_FILE!" >nul 2>&1
if !ERRORLEVEL! equ 0 set GENERATOR_SUPPORTED=1
del "!CMAKE_HELP_FILE!" 2>nul

if not defined GENERATOR_SUPPORTED (
    if "!FORCE_GENERATOR!"=="1" (
        echo.
        echo ERROR: The selected CMake does not support generator "!GENERATOR!".
        echo        Install a newer CMake or use a supported Visual Studio version.
        exit /b 1
    ) else (
        echo.
        echo NOTE: "!GENERATOR!" is not supported by the selected CMake.
        echo       Falling back to Ninja generator.
        set GENERATOR=Ninja
    )
)

REM ----------------------------------------------------------------------
REM Setup Visual Studio environment
REM This is required for the Ninja generator and good practice for VS too.
REM ----------------------------------------------------------------------
if defined VS_PATH (
    set VCVARSALL=!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat
    if exist "!VCVARSALL!" (
        echo.
        echo Setting up Visual Studio x64 environment...
        call "!VCVARSALL!" x64 >nul
        if !ERRORLEVEL! neq 0 (
            echo ERROR: Failed to setup Visual Studio environment.
            exit /b 1
        )
    ) else (
        echo.
        echo WARNING: vcvarsall.bat not found at:
        echo   !VCVARSALL!
        if "!GENERATOR!"=="Ninja" (
            echo ERROR: Ninja generator requires the Visual Studio environment.
            exit /b 1
        )
    )
) else (
    if "!GENERATOR!"=="Ninja" (
        echo.
        echo WARNING: No Visual Studio installation detected for Ninja generator.
        echo If you are already running inside a VS Developer Prompt, this is fine.
        echo Otherwise, please install Visual Studio or run this script from a
        echo Developer Prompt.
    )
)

echo.
echo Using generator: !GENERATOR!
echo Build type:      !BUILD_TYPE!
echo.

REM ----------------------------------------------------------------------
REM Configure CMake
REM ----------------------------------------------------------------------
if not exist "build" mkdir build
cd build

if "!GENERATOR!"=="Ninja" (
    !CMAKE_EXE! -G "!GENERATOR!" -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ..
) else (
    !CMAKE_EXE! -G "!GENERATOR!" -A x64 ..
)

if !ERRORLEVEL! neq 0 (
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
if not "!GENERATOR!"=="Ninja" (
    echo   1. Open build\Wuya.slnx in Visual Studio
    echo   2. Build the solution in your IDE
)
echo   Or use: cmake --build build --config !BUILD_TYPE!
echo.

cd ..
endlocal
