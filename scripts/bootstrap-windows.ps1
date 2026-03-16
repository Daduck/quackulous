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

  throw "Unable to find $CommandName."
}

$repoRoot = Split-Path -Parent $PSScriptRoot
$cmake = Resolve-Tool -CommandName "cmake" -Candidates @(
  "C:\Program Files\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
  "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
)

$vcpkgRoot = $env:VCPKG_ROOT
if (-not $vcpkgRoot) {
  $vcpkgRoot = Join-Path $repoRoot ".tools\vcpkg"
}

if (-not (Test-Path $vcpkgRoot)) {
  git clone https://github.com/microsoft/vcpkg $vcpkgRoot
}

$vcpkgExe = Join-Path $vcpkgRoot "vcpkg.exe"
if (-not (Test-Path $vcpkgExe)) {
  & (Join-Path $vcpkgRoot "bootstrap-vcpkg.bat")
}

& $vcpkgExe install sdl2:x64-windows

$fullBuildDir = Join-Path $repoRoot $BuildDir
$toolchain = Join-Path $vcpkgRoot "scripts\buildsystems\vcpkg.cmake"

& $cmake -S $repoRoot -B $fullBuildDir -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=$toolchain `
  -DQUACK_BUILD_RENDERER_OPENGL2=OFF

if (-not $SkipBuild) {
  & $cmake --build $fullBuildDir --config $Configuration
}

Write-Host "Runtime output: $(Join-Path $fullBuildDir "stage\$Configuration")"
