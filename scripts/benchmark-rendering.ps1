param(
    [string]$Renderer = "opengl2",
    [string]$DemoName,
    [string]$Config = "Debug",
    [string]$StageDir = "build\windows-msvc\stage",
    [switch]$Help
)

if ($Help) {
    Write-Host "Quackulous Renderer Benchmark Script"
    Write-Host "===================================="
    Write-Host "Usage:"
    Write-Host "  .\scripts\benchmark-rendering.ps1 -DemoName <demo> [-Renderer opengl2|opengl1]"
    Write-Host ""
    Write-Host "How to record a demo if you don't have one:"
    Write-Host "  1. Launch the game normally."
    Write-Host "  2. Load a map via the console: \devmap tremor"
    Write-Host "  3. Type: \record mybench"
    Write-Host "  4. Run around for a bit, look at heavy geometry/lighting."
    Write-Host "  5. Type: \stoprecord"
    Write-Host "  6. Now you can run: .\scripts\benchmark-rendering.ps1 -DemoName mybench"
    exit 0
}

if (-not $DemoName) {
    Write-Error "Please specify a -DemoName. Run with -Help for instructions on how to record one."
    exit 1
}

$exePath = Join-Path $StageDir $Config "tremulous.x86_64.exe"
if (-not (Test-Path $exePath)) {
    Write-Error "Could not find $exePath - have you built the project in $Config?"
    exit 1
}

$logFile = "benchmark_${Renderer}_${DemoName}.log"
if (Test-Path $logFile) { Remove-Item $logFile }

Write-Host "Benchmarking $DemoName using $Renderer..."
Write-Host "Launching client. This will run headless or very quickly and automatically exit..."

# We run the client with timedemo 1. It will execute the demo as fast as possible.
# +set cl_timedemoLog outputs frame durations. 
# +quit will quit immediately after the demo completes.
$args = @(
    "+set", "cl_renderer", $Renderer,
    "+set", "timedemo", "1",
    "+set", "cl_timedemoLog", $logFile,
    "+demo", $DemoName,
    "+quit"
)

# Run process and wait for it to exit
$process = Start-Process -FilePath $exePath -ArgumentList $args -NoNewWindow -Wait -PassThru

if ($process.ExitCode -ne 0) {
    Write-Warning "Client exited with code $($process.ExitCode)"
}

if (-not (Test-Path $logFile)) {
    Write-Error "Benchmark failed to produce log file: $logFile"
    exit 1
}

# Parse the timedemo log to compute FPS
# The log file contains the duration of each frame in milliseconds.
$durations = Get-Content $logFile | Where-Object { $_ -match '^\d+$' } | ForEach-Object { [int]$_ }

if ($durations.Count -eq 0) {
    Write-Error "Log file $logFile is empty or invalid."
    exit 1
}

$totalMs = 0
$minMs = [int]::MaxValue
$maxMs = 0

foreach ($d in $durations) {
    $totalMs += $d
    if ($d -lt $minMs) { $minMs = $d }
    if ($d -gt $maxMs) { $maxMs = $d }
}

$avgMs = $totalMs / $durations.Count
$avgFps = if ($avgMs -gt 0) { 1000.0 / $avgMs } else { 0 }
$maxFps = if ($minMs -gt 0) { 1000.0 / $minMs } else { 0 }
$minFps = if ($maxMs -gt 0) { 1000.0 / $maxMs } else { 0 }

Write-Host ""
Write-Host "--- Benchmark Results ($Renderer) ---"
Write-Host "Frames:    $($durations.Count)"
Write-Host "Time:      $($totalMs) ms"
Write-Host "Avg FPS:   $("{0:N2}" -f $avgFps)"
Write-Host "Min FPS:   $("{0:N2}" -f $minFps) ($maxMs ms)"
Write-Host "Max FPS:   $("{0:N2}" -f $maxFps) ($minMs ms)"
Write-Host "--------------------------------------"
