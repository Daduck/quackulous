Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.IO.Compression.FileSystem

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$pk3Path = Join-Path $repoRoot "gamedata\base\data-1.1.0.pk3"
$referenceDir = Join-Path $PSScriptRoot "reference\titan-originals"
$overrideDir = Join-Path $repoRoot "assets\textures\titan"
$boardPath = Join-Path $PSScriptRoot "reference\titan-refresh-board.jpg"

$textureNames = @(
  "basic_panel_b.jpg",
  "dark_metal.jpg",
  "dark_metal_2.jpg",
  "multipanels.jpg",
  "multipanels_2.jpg",
  "support_1.jpg",
  "support_2.jpg",
  "grate.jpg",
  "grate_b.jpg",
  "grate_edge.jpg"
)

function Ensure-Directory {
  param([string]$Path)
  if (-not (Test-Path $Path)) {
    New-Item -ItemType Directory -Path $Path -Force | Out-Null
  }
}

function Extract-ReferenceTextures {
  Ensure-Directory -Path $referenceDir

  $archive = [IO.Compression.ZipFile]::OpenRead($pk3Path)
  try {
    foreach ($name in $textureNames) {
      $entryName = "textures/titan/$name"
      $entry = $archive.Entries | Where-Object FullName -eq $entryName
      if (-not $entry) {
        throw "Missing titan texture in PK3: $entryName"
      }

      $destination = Join-Path $referenceDir $name
      if (-not (Test-Path $destination)) {
        [IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $destination, $false)
      }
    }
  }
  finally {
    $archive.Dispose()
  }
}

function Clamp-Byte {
  param([double]$Value)
  if ($Value -lt 0) { return 0 }
  if ($Value -gt 255) { return 255 }
  return [int][Math]::Round($Value)
}

function Enhance-Channel {
  param(
    [int]$Value,
    [double]$BlurredValue,
    [double]$Contrast,
    [double]$UnsharpAmount
  )

  $centered = (($Value / 255.0) - 0.5) * $Contrast + 0.5
  $contrastAdjusted = $centered * 255.0
  $enhanced = $contrastAdjusted + ($Value - $BlurredValue) * $UnsharpAmount
  return Clamp-Byte -Value $enhanced
}

function New-EnhancedBitmap {
  param([string]$SourcePath)

  $source = [System.Drawing.Bitmap]::FromFile($SourcePath)
  try {
    $targetWidth = $source.Width * 2
    $targetHeight = $source.Height * 2
    $enhanced = New-Object System.Drawing.Bitmap($targetWidth, $targetHeight)
    $graphics = [System.Drawing.Graphics]::FromImage($enhanced)
    try {
      $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
      $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
      $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
      $contrast = 1.06
      $translate = (1.0 - $contrast) * 0.5
      $imageAttributes = New-Object System.Drawing.Imaging.ImageAttributes
      try {
        $matrix = New-Object System.Drawing.Imaging.ColorMatrix(, (
          [single[]]($contrast, 0, 0, 0, 0),
          [single[]](0, $contrast, 0, 0, 0),
          [single[]](0, 0, $contrast, 0, 0),
          [single[]](0, 0, 0, 1, 0),
          [single[]]($translate, $translate, $translate, 0, 1)
        ))
        $imageAttributes.SetColorMatrix($matrix)
        $graphics.DrawImage(
          $source,
          [System.Drawing.Rectangle]::FromLTRB(0, 0, $targetWidth, $targetHeight),
          0,
          0,
          $source.Width,
          $source.Height,
          [System.Drawing.GraphicsUnit]::Pixel,
          $imageAttributes
        )
      }
      finally {
        $imageAttributes.Dispose()
      }
    }
    finally {
      $graphics.Dispose()
    }

    return $enhanced
  }
  finally {
    $source.Dispose()
  }
}

function Write-RefreshOverrides {
  Ensure-Directory -Path $overrideDir

  foreach ($name in $textureNames) {
    $sourcePath = Join-Path $referenceDir $name
    $destination = Join-Path $overrideDir $name

    $bitmap = New-EnhancedBitmap -SourcePath $sourcePath
    try {
      $bitmap.Save($destination, [System.Drawing.Imaging.ImageFormat]::Jpeg)
    }
    finally {
      $bitmap.Dispose()
    }
  }
}

function Write-ReferenceBoard {
  $cellWidth = 512
  $cellHeight = 512
  $margin = 24
  $labelHeight = 24
  $columns = 2
  $rows = $textureNames.Count
  $boardWidth = ($cellWidth * $columns) + ($margin * 3)
  $boardHeight = ($rows * ($cellHeight + $labelHeight + $margin)) + $margin

  $board = New-Object System.Drawing.Bitmap($boardWidth, $boardHeight)
  $graphics = [System.Drawing.Graphics]::FromImage($board)
  $font = New-Object System.Drawing.Font("Segoe UI", 10, [System.Drawing.FontStyle]::Regular)
  $brush = [System.Drawing.Brushes]::White
  $background = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(22, 24, 28))
  try {
    $graphics.Clear([System.Drawing.Color]::FromArgb(22, 24, 28))

    for ($i = 0; $i -lt $textureNames.Count; $i++) {
      $name = $textureNames[$i]
      $top = $margin + ($i * ($cellHeight + $labelHeight + $margin))
      $leftOriginal = $margin
      $leftRefresh = $margin * 2 + $cellWidth

      $original = [System.Drawing.Image]::FromFile((Join-Path $referenceDir $name))
      $refresh = [System.Drawing.Image]::FromFile((Join-Path $overrideDir $name))
      try {
        $graphics.FillRectangle($background, $leftOriginal - 2, $top - 2, $cellWidth + 4, $cellHeight + 4)
        $graphics.FillRectangle($background, $leftRefresh - 2, $top - 2, $cellWidth + 4, $cellHeight + 4)
        $graphics.DrawImage($original, $leftOriginal, $top, $cellWidth, $cellHeight)
        $graphics.DrawImage($refresh, $leftRefresh, $top, $cellWidth, $cellHeight)
        $graphics.DrawString("$name (original)", $font, $brush, [float]$leftOriginal, [float]($top + $cellHeight + 2))
        $graphics.DrawString("$name (refresh)", $font, $brush, [float]$leftRefresh, [float]($top + $cellHeight + 2))
      }
      finally {
        $original.Dispose()
        $refresh.Dispose()
      }
    }

    $board.Save($boardPath, [System.Drawing.Imaging.ImageFormat]::Jpeg)
  }
  finally {
    $background.Dispose()
    $font.Dispose()
    $graphics.Dispose()
    $board.Dispose()
  }
}

if (-not (Test-Path $pk3Path)) {
  throw "PK3 not found: $pk3Path"
}

Extract-ReferenceTextures
Write-RefreshOverrides
Write-ReferenceBoard

Write-Output "Refreshed titan textures written to: $overrideDir"
Write-Output "Reference board written to: $boardPath"
