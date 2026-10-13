@echo off
setlocal enabledelayedexpansion
title ManiMart Marketplace Server

:: 1. Navigate to project root directory
cd /d "%~dp0"

:: 2. Set Toolchain & Library DLL Paths
set "PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%"

echo ======================================================================
echo   ManiMart - Full-Stack Multi-Seller E-Commerce Marketplace
echo ======================================================================
echo.

:: 3. Check if backend executable exists; if not, build it automatically
if not exist "build_test\manimart.exe" (
    echo [INFO] Backend executable not found. Building project with CMake & Ninja...
    cmake -B build_test -G "Ninja" -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe"
    cmake --build build_test --target manimart
    if errorlevel 1 (
        echo [ERROR] Build failed! Please inspect compilation errors above.
        pause
        exit /b 1
    )
    echo [INFO] Build succeeded!
    echo.
)

:: 4. Print Startup Details
echo [1/2] Launching ManiMart Backend Server on http://127.0.0.1:8080 ...
echo [2/2] Opening browser at http://127.0.0.1:8080/ ...
echo.
echo ======================================================================
echo   ManiMart is RUNNING at:
echo   - Website / Frontend: http://127.0.0.1:8080/
echo   - Health Endpoint:    http://127.0.0.1:8080/api/v1/health
echo   - API Base URL:       http://127.0.0.1:8080/api/v1/
echo.
echo   DEMO ACCOUNTS:
echo   - Buyer:  buyer1@gmail.com    / BuyerPass@123
echo   - Seller: seller1@techhub.com / SellerPass@123
echo   - Admin:  admin@manimart.com  / AdminPass@123
echo.
echo   Press Ctrl+C in this terminal window to stop the server anytime.
echo ======================================================================
echo.

:: 5. Open browser to http://127.0.0.1:8080/ in a detached process
start "" "http://127.0.0.1:8080/"

:: 6. Run the ManiMart Backend Process
build_test\manimart.exe

if errorlevel 1 (
    echo [ERROR] ManiMart server stopped unexpectedly with exit code %errorlevel%.
)

pause
