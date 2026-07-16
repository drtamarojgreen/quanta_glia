@echo off
rem ==============================================================================
rem QuantaGlia Windows Installer Launcher
rem
rem This script launches the PowerShell installer under Windows.
rem ==============================================================================

echo Launching QuantaGlia PowerShell Installer...
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0install.ps1"
if %errorlevel% neq 0 (
    echo Error: Installer failed with code %errorlevel%
    pause
    exit /b %errorlevel%
)
pause
