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
$serverExe = Join-Path $runtimeRootPath "tremded.x86_64.exe"

$serverHomeRoot = Join-Path $runtimeRootPath "testhome\server"
$clientHomeRoot = Join-Path $runtimeRootPath "testhome\client"

$serverLogPath = Join-Path $serverHomeRoot "base\qconsole.log"
$clientLogPath = Join-Path $clientHomeRoot "base\qconsole.log"

if (-not (Test-Path -LiteralPath $clientExe)) {
  throw "Client executable not found at '$clientExe'."
}

if (-not (Test-Path -LiteralPath $serverExe)) {
  throw "Server executable not found at '$serverExe'."
}

New-Item -ItemType Directory -Path (Join-Path $serverHomeRoot "base") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $clientHomeRoot "base") -Force | Out-Null

if (Test-Path -LiteralPath $serverLogPath) { Remove-Item -LiteralPath $serverLogPath -Force }
if (Test-Path -LiteralPath $clientLogPath) { Remove-Item -LiteralPath $clientLogPath -Force }

# Launch Dedicated Server
$serverArgs = @(
  "+set", "fs_basepath", $runtimeRootPath,
  "+set", "fs_homepath", $serverHomeRoot,
  "+set", "logfile", "2",
  "+set", "com_abnormalExit", "0",
  "+set", "sv_pure", "0",
  "+set", "vm_game", "0",
  "+set", "net_port", "27961",
  "+map", $Map
)

$serverProcess = Start-Process -FilePath $serverExe -ArgumentList $serverArgs -WorkingDirectory $launchWorkingDirectory -PassThru

# Give the server a couple of seconds to bind the port and load the map
Start-Sleep -Seconds 2

# Launch Client
$clientArgs = @(
  "+set", "fs_basepath", $runtimeRootPath,
  "+set", "fs_homepath", $clientHomeRoot,
  "+set", "logfile", "2",
  "+set", "com_abnormalExit", "0",
  "+set", "com_introplayed", "1",
  "+set", "s_initsound", "0",
  "+set", "sv_pure", "0",
  "+set", "vm_ui", "0",
  "+set", "vm_cgame", "0",
  "+set", "r_fullscreen", "0",
  "+connect", "127.0.0.1:27961"
)

$clientProcess = Start-Process -FilePath $clientExe -ArgumentList $clientArgs -WorkingDirectory $launchWorkingDirectory -PassThru

$startupDeadline = (Get-Date).AddSeconds($StartupTimeoutSeconds)
$clientLogText = ""
$enteredGame = $false

try {
  while ((Get-Date) -lt $startupDeadline) {
    if ($clientProcess.HasExited) {
      break
    }

    if (Test-Path -LiteralPath $clientLogPath) {
      $clientLogText = Get-Content -LiteralPath $clientLogPath -Raw
      if ($clientLogText -match "entered the game") {
        $enteredGame = $true
        break
      }
    }

    Start-Sleep -Milliseconds 500
    $clientProcess.Refresh()
  }

  # Shutdown Client
  if (-not $clientProcess.HasExited) {
    $clientProcess.Refresh()
    if ($clientProcess.MainWindowHandle -ne 0) {
      $null = $clientProcess.CloseMainWindow()
      if (-not $clientProcess.WaitForExit($ShutdownTimeoutSeconds * 1000)) {
        Stop-Process -Id $clientProcess.Id -Force
        $clientProcess.WaitForExit()
      }
    }
    else {
      Stop-Process -Id $clientProcess.Id -Force
      $clientProcess.WaitForExit()
    }
  }

  # Shutdown Server
  if (-not $serverProcess.HasExited) {
    Stop-Process -Id $serverProcess.Id -Force
    $serverProcess.WaitForExit()
  }
}
finally {
  if (-not $clientProcess.HasExited) {
    Stop-Process -Id $clientProcess.Id -Force
    $clientProcess.WaitForExit()
  }
  if (-not $serverProcess.HasExited) {
    Stop-Process -Id $serverProcess.Id -Force
    $serverProcess.WaitForExit()
  }
}

if (-not (Test-Path -LiteralPath $clientLogPath)) {
  throw "Multiplayer smoke test did not produce client log '$clientLogPath'."
}

$clientLogText = Get-Content -LiteralPath $clientLogPath -Raw

if (-not $enteredGame -and $clientLogText -notmatch "entered the game") {
  throw "Multiplayer smoke test client never reached local gameplay. See '$clientLogPath'."
}

Write-Host "Multiplayer connection successful!"
