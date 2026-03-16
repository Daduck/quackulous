param(
  [Parameter(Mandatory = $true)]
  [string]$RuntimeRoot,
  [Parameter(Mandatory = $true)]
  [string]$Arch,
  [string]$RequireServer = "1",
  [string]$RequireOpenGL2 = "0"
)

$ErrorActionPreference = "Stop"

$requiredFiles = @(
  (Join-Path $RuntimeRoot "tremulous.$Arch.exe"),
  (Join-Path $RuntimeRoot "renderer_opengl1_${Arch}.dll"),
  (Join-Path $RuntimeRoot "base\cgame$Arch.dll"),
  (Join-Path $RuntimeRoot "base\game$Arch.dll"),
  (Join-Path $RuntimeRoot "base\ui$Arch.dll"),
  (Join-Path $RuntimeRoot "base\ui\main.menu"),
  (Join-Path $RuntimeRoot "base\configs\classes\builder.cfg"),
  (Join-Path $RuntimeRoot "server.cfg")
)

if ($RequireServer -eq "1") {
  $requiredFiles += Join-Path $RuntimeRoot "tremded.$Arch.exe"
}

if ($RequireOpenGL2 -eq "1") {
  $requiredFiles += Join-Path $RuntimeRoot "renderer_opengl2_${Arch}.dll"
}

$missing = $requiredFiles | Where-Object { -not (Test-Path $_) }
if ($missing.Count -gt 0) {
  Write-Error ("Missing runtime artifacts:`n" + ($missing -join "`n"))
}
