# ==============================================================================
# QuantaGlia Unified Installer (Windows PowerShell)
#
# This script automates the process of building the quanta_glia (glia) C++ toolkit
# and adding it to the user's PATH under Windows.
#
# Usage:
#   powershell -ExecutionPolicy Bypass -File scripts/install.ps1
# ==============================================================================

$ErrorActionPreference = "Stop"

Write-Host "====================================================" -ForegroundColor Blue
Write-Host "         QuantaGlia C++ Installer (Windows)         " -ForegroundColor Blue
Write-Host "====================================================" -ForegroundColor Blue

# --- Dependency Check ---
Write-Host "`n[1/5] Checking prerequisites..." -ForegroundColor Yellow

# Check for CMake
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Error "❌ Error: cmake is not installed or not in your PATH. Please install CMake and try again."
    exit 1
} else {
    $cmakeVersion = (cmake --version | Select-Object -First 1)
    Write-Host "✓ Found CMake: $cmakeVersion" -ForegroundColor Green
}

# --- Compilation ---
Write-Host "`n[2/5] Compiling QuantaGlia..." -ForegroundColor Yellow

$RepoRoot = Resolve-Path "$PSScriptRoot\.."
$BuildDir = Join-Path $RepoRoot "build"

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

Set-Location $BuildDir

Write-Host "Configuring project in $BuildDir..."
# Let CMake automatically choose the best available compiler generator on Windows
cmake ..

Write-Host "Building 'glia' and 'sorrel_test' binaries in Release configuration..."
cmake --build . --config Release

$GliaBinary = Join-Path $BuildDir "Release\glia.exe"
if (-not (Test-Path $GliaBinary)) {
    # Fallback to single-configuration build location if no Release folder (e.g. MinGW)
    $GliaBinary = Join-Path $BuildDir "glia.exe"
}

if (-not (Test-Path $GliaBinary)) {
    Write-Error "❌ Error: 'glia.exe' executable was not built successfully."
    exit 1
}

Write-Host "✓ Compilation successful!" -ForegroundColor Green

# --- Verification / Testing ---
Write-Host "`n[3/5] Running verification tests..." -ForegroundColor Yellow
$TestBinary = Join-Path $BuildDir "Release\sorrel_test.exe"
if (-not (Test-Path $TestBinary)) {
    $TestBinary = Join-Path $BuildDir "sorrel_test.exe"
}

if (Test-Path $TestBinary) {
    Write-Host "Running sorrel_test verification suite..."
    & $TestBinary
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ All verification tests passed perfectly!" -ForegroundColor Green
    } else {
        Write-Host "⚠️ Warning: Some verification tests failed. Proceeding with installation anyway." -ForegroundColor Yellow
    }
} else {
    Write-Host "ℹ️ No test binary found. Skipping test phase." -ForegroundColor Cyan
}

# --- Installation ---
Write-Host "`n[4/5] Installing glia binary..." -ForegroundColor Yellow
$InstallDir = Join-Path $Home ".glia\bin"

if (-not (Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Path $InstallDir | Out-Null
}

Copy-Item $GliaBinary (Join-Path $InstallDir "glia.exe") -Force
Write-Host "✓ Installed 'glia.exe' to (Join-Path $InstallDir 'glia.exe')" -ForegroundColor Green

# --- Path Configuration ---
Write-Host "`n[5/5] Configuring Environment PATH..." -ForegroundColor Yellow

$UserPath = [Environment]::GetEnvironmentVariable("Path", "User")
$NormalizedUserPaths = $UserPath -split ';' | ForEach-Object { $_.Trim().TrimEnd('\') }
$NormalizedInstallDir = $InstallDir.Trim().TrimEnd('\')

if ($NormalizedUserPaths -contains $NormalizedInstallDir) {
    Write-Host "✓ '$InstallDir' is already in your User PATH!" -ForegroundColor Green
} else {
    Write-Host "Adding '$InstallDir' to your User PATH registry..." -ForegroundColor Yellow
    $NewUserPath = "$UserPath;$InstallDir"
    [Environment]::SetEnvironmentVariable("Path", $NewUserPath, "User")
    Write-Host "✓ Successfully added '$InstallDir' to User PATH!" -ForegroundColor Green
    Write-Host "ℹ️ Please restart your terminal/IDE for the PATH changes to take effect." -ForegroundColor Cyan
}

Write-Host "`n====================================================" -ForegroundColor Green
Write-Host "           QuantaGlia Build Completed!             " -ForegroundColor Green
Write-Host "====================================================" -ForegroundColor Green
