param(
  [string]$BspPath = "D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp",
  [switch]$NoBuild
)

$ErrorActionPreference = "Stop"

$projectRoot = $PSScriptRoot
$projectPath = Join-Path $projectRoot "MapEditor\MapEditor.csproj"
$outputDir = Join-Path $projectRoot "MapEditor\bin\Debug\net10.0-windows"
$exePath = Join-Path $outputDir "MapEditor.exe"

if (-not (Test-Path $projectPath)) {
  throw "MapEditor project not found: $projectPath"
}

if (-not (Test-Path $BspPath)) {
  throw "BSP file not found: $BspPath"
}

if (-not $NoBuild) {
  dotnet build $projectPath -c Debug | Out-Host
}

if (-not (Test-Path $exePath)) {
  throw "MapEditor executable not found: $exePath"
}

Get-Process MapEditor -ErrorAction SilentlyContinue | Stop-Process -Force

$argumentLine = "--bsp `"$BspPath`""
Start-Process -FilePath $exePath -ArgumentList $argumentLine -WorkingDirectory $outputDir
Write-Host "Started MapEditor with: $BspPath"
