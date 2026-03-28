param(
  [Parameter(Mandatory = $true)]
  [string]$RuntimeRoot,
  [string]$Map = "tremor",
  [int]$StartupTimeoutSeconds = 45,
  [int]$ShutdownTimeoutSeconds = 15
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$runtimeRootPath = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$launchWorkingDirectory = (Resolve-Path -LiteralPath (Join-Path $runtimeRootPath "..\\..")).Path
$clientExe = Join-Path $runtimeRootPath "tremulous.x86_64.exe"
$logPath = Join-Path $runtimeRootPath "base\\qconsole.log"
$pidFilePath = Join-Path $runtimeRootPath "tremulous.pid"

if (-not (Test-Path -LiteralPath $clientExe)) {
  throw "Client executable not found at '$clientExe'."
}

if (Test-Path -LiteralPath $logPath) {
  Remove-Item -LiteralPath $logPath -Force
}

if (Test-Path -LiteralPath $pidFilePath) {
  Remove-Item -LiteralPath $pidFilePath -Force
}

$clientArgs = @(
  "+set", "fs_basepath", $runtimeRootPath,
  "+set", "fs_homepath", $runtimeRootPath,
  "+set", "logfile", "2",
  "+set", "com_introplayed", "1",
  "+set", "s_initsound", "0",
  "+set", "sv_pure", "0",
  "+set", "vm_ui", "0",
  "+set", "vm_cgame", "0",
  "+set", "vm_game", "0",
  "+set", "r_fullscreen", "0",
  "+map", $Map
)

$process = Start-Process -FilePath $clientExe -ArgumentList $clientArgs -WorkingDirectory $launchWorkingDirectory -PassThru

$startupDeadline = (Get-Date).AddSeconds($StartupTimeoutSeconds)
$logText = ""
$enteredGame = $false

try {
  while ((Get-Date) -lt $startupDeadline) {
    if ($process.HasExited) {
      break
    }

    if (Test-Path -LiteralPath $logPath) {
      $logText = Get-Content -LiteralPath $logPath -Raw
      if ($logText -match "entered the game") {
        $enteredGame = $true
        break
      }
    }

    Start-Sleep -Milliseconds 500
    $process.Refresh()
  }

  if (-not $process.HasExited) {
    $process.Refresh()
    if ($process.MainWindowHandle -ne 0) {
      $null = $process.CloseMainWindow()
      if (-not $process.WaitForExit($ShutdownTimeoutSeconds * 1000)) {
        Stop-Process -Id $process.Id -Force
        $process.WaitForExit()
      }
    }
    else {
      Stop-Process -Id $process.Id -Force
      $process.WaitForExit()
    }
  }
}
finally {
  if (-not $process.HasExited) {
    Stop-Process -Id $process.Id -Force
    $process.WaitForExit()
  }

  if (Test-Path -LiteralPath $pidFilePath) {
    Remove-Item -LiteralPath $pidFilePath -Force
  }
}

if (-not (Test-Path -LiteralPath $logPath)) {
  throw "Startup smoke test did not produce '$logPath'."
}

$logText = Get-Content -LiteralPath $logPath -Raw

if (-not $enteredGame -and $logText -notmatch "entered the game") {
  throw "Startup smoke test never reached local gameplay. See '$logPath'."
}

$requiredPatterns = @(
  "----- Client Initialization Complete -----",
  "Loading DLL file: .*uix86_64\.dll",
  "Loading DLL file: .*gamex86_64\.dll",
  "Loading DLL file: .*cgamex86_64\.dll",
  "Server: $([Regex]::Escape($Map))",
  "map-$([Regex]::Escape($Map))-1\.1\.0\.pk3",
  "entered the game"
)

foreach ($pattern in $requiredPatterns) {
  if ($logText -notmatch $pattern) {
    throw "Startup smoke test is missing expected log pattern '$pattern'. See '$logPath'."
  }
}

$forbiddenPatterns = @(
  "Loading vm file vm/ui\.qvm",
  "Loading vm file vm/cgame\.qvm",
  "Loading vm file vm/game\.qvm",
  "Client fatal crashed:",
  "Server fatal crashed:",
  "Server crashed:"
)

foreach ($pattern in $forbiddenPatterns) {
  if ($logText -match $pattern) {
    throw "Startup smoke test hit forbidden log pattern '$pattern'. See '$logPath'."
  }
}
