param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Debug",
  [string]$BuildDir = "build/windows-msvc",
  [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

function Resolve-Tool {
  param(
    [string]$CommandName,
    [string[]]$Candidates
  )

  $command = Get-Command $CommandName -ErrorAction SilentlyContinue
  if ($command) {
    return $command.Source
  }

  foreach ($candidate in $Candidates) {
    if (Test-Path $candidate) {
      return $candidate
    }
  }

  if ($CommandName -eq "cmake") {
    throw @"
Unable to find cmake.

Install one of these first:
- Visual Studio 2022 with the 'Desktop development with C++' workload and CMake tools
- Standalone CMake from https://cmake.org/download/

Then reopen the terminal and run:
powershell -ExecutionPolicy Bypass -File .\scripts\bootstrap-windows.ps1 -Configuration Debug
"@
  }

  throw "Unable to find $CommandName."
}

function Find-VsWhere {
  $candidates = @(
    "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe",
    "C:\Program Files\Microsoft Visual Studio\Installer\vswhere.exe"
  )

  foreach ($candidate in $candidates) {
    if (Test-Path $candidate) {
      return $candidate
    }
  }

  return $null
}

function Get-VisualStudioInstance {
  $vswhere = Find-VsWhere
  if (-not $vswhere) {
    throw @"
Unable to find vswhere.exe, so Visual Studio cannot be validated.

Install Visual Studio 2022 or 2026 (or Build Tools) with:
- Desktop development with C++
- MSVC build tools for x64/x86
- C++ CMake tools for Windows

Then reopen the terminal and run:
powershell -ExecutionPolicy Bypass -File .\scripts\bootstrap-windows.ps1 -Configuration Debug
"@
  }

  $instanceJson = & $vswhere -latest -products * -version "[17.0,19.0)" `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -format json

  $instance = ($instanceJson | ConvertFrom-Json) | Select-Object -First 1
  if (-not $instance) {
    throw @"
Unable to find a usable Visual Studio instance (2022 or 2026) with MSVC.

Install Visual Studio 2022 or 2026 (or Build Tools) with:
- Desktop development with C++
- MSVC build tools for x64/x86
- C++ CMake tools for Windows

Then reopen the terminal and run:
powershell -ExecutionPolicy Bypass -File .\scripts\bootstrap-windows.ps1 -Configuration Debug
"@
  }

  return $instance
}

function Get-CMakeGenerator {
  param([object]$VsInstance)
  $major = [int]($VsInstance.installationVersion -split '\.')[0]
  switch ($major) {
    17 { return "Visual Studio 17 2022" }
    18 { return "Visual Studio 18 2026" }
    default { throw "Unsupported Visual Studio major version: $major" }
  }
}

function Reset-StaleCMakeState {
  param(
    [string]$BuildDirectory,
    [string]$ExpectedGenerator
  )

  $cachePath = Join-Path $BuildDirectory "CMakeCache.txt"
  if (-not (Test-Path $cachePath)) {
    return
  }

  $cacheContent = Get-Content $cachePath -Raw
  $hasLiteralToolchain = $cacheContent -match '(?m)^CMAKE_TOOLCHAIN_FILE:.*=\$toolchain$'
  $hasWrongGenerator = $cacheContent -match "(?m)^CMAKE_GENERATOR:INTERNAL=(?!$([regex]::Escape($ExpectedGenerator))`$)"

  if ($hasLiteralToolchain -or $hasWrongGenerator) {
    Write-Host "Removing stale CMake state from $BuildDirectory"
    Remove-Item $BuildDirectory -Recurse -Force
  }
}

$repoRoot = Split-Path -Parent $PSScriptRoot
$vsInstance = Get-VisualStudioInstance
$generator = Get-CMakeGenerator -VsInstance $vsInstance
$vsInstallPath = $vsInstance.installationPath

$cmake = Resolve-Tool -CommandName "cmake" -Candidates @(
  "C:\Program Files\CMake\bin\cmake.exe",
  "$vsInstallPath\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\18\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
)

$vcpkgRoot = Join-Path $repoRoot ".tools\vcpkg"

if (-not (Test-Path $vcpkgRoot)) {
  git clone https://github.com/microsoft/vcpkg $vcpkgRoot
}

$vcpkgExe = Join-Path $vcpkgRoot "vcpkg.exe"
if (-not (Test-Path $vcpkgExe)) {
  & (Join-Path $vcpkgRoot "bootstrap-vcpkg.bat")
}

& $vcpkgExe install sdl2:x64-windows openal-soft:x64-windows

$fullBuildDir = Join-Path $repoRoot $BuildDir
$toolchain = Join-Path $vcpkgRoot "scripts\buildsystems\vcpkg.cmake"
if (-not (Test-Path $toolchain)) {
  throw "Unable to find the vcpkg toolchain file at $toolchain"
}

Reset-StaleCMakeState -BuildDirectory $fullBuildDir -ExpectedGenerator $generator

$cmakeConfigureArgs = @(
  "-S", $repoRoot,
  "-B", $fullBuildDir,
  "-G", $generator,
  "-A", "x64",
  "-DCMAKE_TOOLCHAIN_FILE=$toolchain",
  "-DQUACK_BUILD_RENDERER_OPENGL2=ON"
)

& $cmake @cmakeConfigureArgs

if (-not $SkipBuild) {
  & $cmake --build $fullBuildDir --config $Configuration
}

Write-Host "Runtime output: $(Join-Path $fullBuildDir "stage\$Configuration")"
