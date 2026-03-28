param(
  [Parameter(Mandatory = $true)]
  [string]$RuntimeRoot,
  [Parameter(Mandatory = $true)]
  [ValidateSet("renderer", "filesystem-network", "audio")]
  [string]$Module,
  [string]$Map = "tremor",
  [int]$StartupTimeoutSeconds = 45,
  [int]$ShutdownTimeoutSeconds = 15
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$runtimeRootPath = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$launchWorkingDirectory = (Resolve-Path -LiteralPath (Join-Path $runtimeRootPath "..\\..")).Path
$clientExe = Join-Path $runtimeRootPath "tremulous.x86_64.exe"
$homeRoot = Join-Path $runtimeRootPath "testhome\\$Module"
$homeBase = Join-Path $homeRoot "base"
$logPath = Join-Path $homeBase "qconsole.log"
$pidFilePath = Join-Path $homeRoot "tremulous.pid"

if (-not (Test-Path -LiteralPath $clientExe)) {
  throw "Client executable not found at '$clientExe'."
}

New-Item -ItemType Directory -Path $homeBase -Force | Out-Null

if (Test-Path -LiteralPath $logPath) {
  Remove-Item -LiteralPath $logPath -Force
}

if (Test-Path -LiteralPath $pidFilePath) {
  Remove-Item -LiteralPath $pidFilePath -Force
}

$soundInit = if ($Module -eq "audio") { "1" } else { "0" }

$clientArgs = @(
  "+set", "fs_basepath", $runtimeRootPath,
  "+set", "fs_homepath", $homeRoot,
  "+set", "logfile", "2",
  "+set", "com_introplayed", "1",
  "+set", "sv_pure", "0",
  "+set", "vm_ui", "0",
  "+set", "vm_cgame", "0",
  "+set", "vm_game", "0",
  "+set", "r_fullscreen", "0",
  "+set", "s_initsound", $soundInit,
  "+map", $Map
)

$requiredPatterns = switch ($Module) {
  "renderer" {
    @(
      "Trying to load ""renderer_opengl1_x86_64\.dll""",
      "SDL using driver """,
      "GL_VENDOR:",
      "GL_RENDERER:",
      "GL_VERSION:",
      "----- finished R_Init -----"
    )
  }
  "filesystem-network" {
    @(
      "We are looking in the current search path:",
      "data-1\.1\.0\.pk3",
      "map-$([Regex]::Escape($Map))-1\.1\.0\.pk3",
      "Winsock Initialized",
      "Opening IP socket:",
      "Loading DLL file: .*gamex86_64\.dll",
      "Server: $([Regex]::Escape($Map))"
    )
  }
  "audio" {
    @(
      "------ Initializing Sound ------",
      "SDL_Init\( SDL_INIT_AUDIO \)\.\.\. OK",
      "SDL audio initialized\.",
      "Sound initialization successful\.",
      "Sound memory manager started"
    )
  }
}

$readyPattern = switch ($Module) {
  "renderer" { "----- finished R_Init -----" }
  "filesystem-network" { "Opening IP socket:" }
  "audio" { "Sound memory manager started" }
}

$forbiddenPatterns = @(
  "Loading vm file vm/ui\.qvm",
  "Loading vm file vm/cgame\.qvm",
  "Loading vm file vm/game\.qvm",
  "Client fatal crashed:",
  "Server fatal crashed:",
  "Server crashed:",
  "recursive error after:",
  "Sys_Error:"
)

$process = Start-Process -FilePath $clientExe -ArgumentList $clientArgs -WorkingDirectory $launchWorkingDirectory -PassThru

$startupDeadline = (Get-Date).AddSeconds($StartupTimeoutSeconds)
$logText = ""
$ready = $false

try {
  while ((Get-Date) -lt $startupDeadline) {
    if ($process.HasExited) {
      break
    }

    if (Test-Path -LiteralPath $logPath) {
      $logText = Get-Content -LiteralPath $logPath -Raw
      if ($logText -match $readyPattern) {
        $ready = $true
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
  throw "Module smoke test '$Module' did not produce '$logPath'."
}

$logText = Get-Content -LiteralPath $logPath -Raw

if (-not $ready -and $logText -notmatch $readyPattern) {
  throw "Module smoke test '$Module' never reached its ready marker '$readyPattern'. See '$logPath'."
}

foreach ($pattern in $requiredPatterns) {
  if ($logText -notmatch $pattern) {
    throw "Module smoke test '$Module' is missing expected log pattern '$pattern'. See '$logPath'."
  }
}

foreach ($pattern in $forbiddenPatterns) {
  if ($logText -match $pattern) {
    throw "Module smoke test '$Module' hit forbidden log pattern '$pattern'. See '$logPath'."
  }
}
