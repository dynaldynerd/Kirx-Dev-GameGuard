param(
  [string]$SummaryPath = '',
  [string]$TsvPath = ''
)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($SummaryPath)) {
  $SummaryPath = Join-Path $repoRoot ("_reports/todo3_goto_summary_{0}.txt" -f (Get-Date -Format 'yyyyMMdd'))
}
if ([string]::IsNullOrWhiteSpace($TsvPath)) {
  $TsvPath = Join-Path $repoRoot ("_reports/todo3_goto_inventory_{0}.tsv" -f (Get-Date -Format 'yyyyMMdd'))
}

$todoItem = (Get-Content (Join-Path $repoRoot 'todo.md') | Where-Object { $_ -match '^3\.' } | Select-Object -First 1).Trim()

$files = Get-ChildItem (Join-Path $repoRoot 'WorldServer') -Recurse -File | Where-Object {
  $_.Extension -in '.cpp', '.h' -and
  $_.FullName -notmatch '\\ThirdParty\\' -and
  $_.FullName -notmatch '\\x64\\' -and
  $_.FullName -notmatch '\\Debug\\' -and
  $_.FullName -notmatch '\\Release\\'
}

function Get-RelativePath {
  param([string]$FullName)

  return $FullName.Substring($repoRoot.Length + 1)
}

function Get-ScannableLine {
  param(
    [string]$Line,
    [ref]$InBlockComment
  )

  $builder = [System.Text.StringBuilder]::new()
  $inString = $false
  $stringQuote = [char]0

  for ($i = 0; $i -lt $Line.Length; ++$i) {
    $ch = $Line[$i]

    if ($InBlockComment.Value) {
      if ($ch -eq '*' -and ($i + 1) -lt $Line.Length -and $Line[$i + 1] -eq '/') {
        $InBlockComment.Value = $false
        ++$i
      }
      continue
    }

    if (-not $inString -and $ch -eq '/' -and ($i + 1) -lt $Line.Length) {
      if ($Line[$i + 1] -eq '/') {
        break
      }
      if ($Line[$i + 1] -eq '*') {
        $InBlockComment.Value = $true
        ++$i
        continue
      }
    }

    if ($inString) {
      if ($ch -eq '\') {
        ++$i
        continue
      }
      if ($ch -eq $stringQuote) {
        $inString = $false
      }
      continue
    }

    if ($ch -eq '"' -or $ch -eq "'") {
      $inString = $true
      $stringQuote = $ch
      continue
    }

    [void]$builder.Append($ch)
  }

  return $builder.ToString()
}

function Get-FunctionSignatureInfo {
  param([string]$Line)

  $trimmed = $Line.Trim()
  if ([string]::IsNullOrWhiteSpace($trimmed)) {
    return $null
  }

  if ($trimmed -match '^(if|for|while|switch|catch|return|else|do)\b') {
    return $null
  }

  if ($trimmed -match '([A-Za-z_][A-Za-z0-9_]*)::(~?[A-Za-z_][A-Za-z0-9_]*)\s*\(') {
    return [pscustomobject]@{
      Class = $Matches[1]
      Function = $Matches[2]
    }
  }

  if ($trimmed -match '^(?:template\s*<.*>\s*)?(?:inline\s+|static\s+|constexpr\s+|virtual\s+|explicit\s+|friend\s+|extern\s+|__forceinline\s+)*[\w:&<>,~\*\s]+\b(~?[A-Za-z_][A-Za-z0-9_]*)\s*\(' -and
      $trimmed -notmatch '=\s*[^,]') {
    return [pscustomobject]@{
      Class = ''
      Function = $Matches[1]
    }
  }

  return $null
}

function Get-BraceDelta {
  param([string]$Line)

  $openCount = ([regex]::Matches($Line, '\{')).Count
  $closeCount = ([regex]::Matches($Line, '\}')).Count
  return $openCount - $closeCount
}

$gotoRegex = [regex]'\bgoto\s+([A-Za-z_$][A-Za-z0-9_$]*)\s*;'
$labelRegex = [regex]'^\s*([A-Za-z_$][A-Za-z0-9_$]*)\s*:\s*$'
$records = [System.Collections.Generic.List[object]]::new()

foreach ($file in $files) {
  $lines = Get-Content $file.FullName
  $relativePath = Get-RelativePath -FullName $file.FullName
  $inBlockComment = $false
  $functionMap = @{}
  $braceDepth = 0
  $currentFunction = $null
  $pendingFunction = $null

  for ($i = 0; $i -lt $lines.Count; ++$i) {
    $rawLine = [string]$lines[$i]
    $codeLine = Get-ScannableLine -Line $rawLine -InBlockComment ([ref]$inBlockComment)
    $signatureInfo = $null
    if (-not $currentFunction -and $braceDepth -eq 0) {
      $signatureInfo = Get-FunctionSignatureInfo -Line $codeLine
      if ($signatureInfo) {
        $pendingFunction = [pscustomobject]@{
          Class = $signatureInfo.Class
          Function = $signatureInfo.Function
          Line = $i + 1
        }
      }
    }

    if ($pendingFunction -and $braceDepth -eq 0 -and $codeLine.Contains('{')) {
      $currentFunction = $pendingFunction
      $pendingFunction = $null
    }

    $trimmedCodeLine = $codeLine.Trim()
    $gotoMatches = $gotoRegex.Matches($codeLine)
    $labelMatch = $labelRegex.Match($codeLine)
    $isLabel = $labelMatch.Success -and
      $trimmedCodeLine -notmatch '^(case|default|public|private|protected)\b'

    if ($gotoMatches.Count -gt 0 -or $isLabel) {
      $fn = if ($currentFunction) {
        $currentFunction
      } else {
        [pscustomobject]@{
          Class = '(unknown)'
          Function = '(unknown)'
          Line = 0
        }
      }

      $key = '{0}|{1}|{2}|{3}' -f $relativePath, $fn.Class, $fn.Function, $fn.Line
      if (-not $functionMap.ContainsKey($key)) {
        $functionMap[$key] = [pscustomobject]@{
          file = $relativePath
          class = $fn.Class
          function = $fn.Function
          function_line = $fn.Line
          first_hit_line = $i + 1
          last_hit_line = $i + 1
          goto_count = 0
          label_count = 0
          labels = [System.Collections.Generic.HashSet[string]]::new()
          sample_line = ($rawLine.Trim() -replace "`t", ' ')
        }
      }

      $entry = $functionMap[$key]
      $entry.last_hit_line = $i + 1
      foreach ($gotoMatch in $gotoMatches) {
        ++$entry.goto_count
        [void]$entry.labels.Add($gotoMatch.Groups[1].Value)
      }
      if ($isLabel) {
        ++$entry.label_count
        [void]$entry.labels.Add($labelMatch.Groups[1].Value)
      }
    }

    $braceDepth += Get-BraceDelta -Line $codeLine
    if ($pendingFunction -and $braceDepth -eq 0 -and $codeLine.Contains(';')) {
      $pendingFunction = $null
    }
    if ($currentFunction -and $braceDepth -eq 0) {
      $currentFunction = $null
    }
  }

  foreach ($entry in $functionMap.Values) {
    $records.Add([pscustomobject]@{
      file = $entry.file
      class = $entry.class
      function = $entry.function
      function_line = $entry.function_line
      first_hit_line = $entry.first_hit_line
      last_hit_line = $entry.last_hit_line
      goto_count = $entry.goto_count
      label_count = $entry.label_count
      total_hits = $entry.goto_count + $entry.label_count
      distinct_label_count = $entry.labels.Count
      labels = (($entry.labels | Sort-Object) -join ', ')
      sample_line = $entry.sample_line
    }) | Out-Null
  }
}

$records = $records | Sort-Object @{Expression = 'total_hits'; Descending = $true}, @{Expression = 'goto_count'; Descending = $true}, file, function, first_hit_line

$tsvLines = [System.Collections.Generic.List[string]]::new()
$tsvLines.Add('file	class	function	function_line	first_hit_line	last_hit_line	goto_count	label_count	total_hits	distinct_label_count	labels	sample_line') | Out-Null
foreach ($record in $records) {
  $values = @(
    [string]$record.file,
    [string]$record.class,
    [string]$record.function,
    [string]$record.function_line,
    [string]$record.first_hit_line,
    [string]$record.last_hit_line,
    [string]$record.goto_count,
    [string]$record.label_count,
    [string]$record.total_hits,
    [string]$record.distinct_label_count,
    [string]$record.labels,
    [string]$record.sample_line
  ) | ForEach-Object { ($_ -replace "`r", ' ' -replace "`n", ' ' -replace "`t", ' ') }
  $tsvLines.Add(($values -join "`t")) | Out-Null
}
Set-Content -Path $TsvPath -Value $tsvLines -Encoding UTF8

$fileSummary = $records | Group-Object file | Sort-Object Count -Descending
$hitSummary = $records | Group-Object file | ForEach-Object {
  [pscustomobject]@{
    file = $_.Name
    functions = $_.Count
    goto_hits = ($_.Group | Measure-Object -Property goto_count -Sum).Sum
    label_hits = ($_.Group | Measure-Object -Property label_count -Sum).Sum
    total_hits = ($_.Group | Measure-Object -Property total_hits -Sum).Sum
  }
} | Sort-Object @{Expression = 'functions'; Descending = $true}, @{Expression = 'total_hits'; Descending = $true}, file

$summary = [System.Collections.Generic.List[string]]::new()
$summary.Add('TODO Item 3 Scan') | Out-Null
$summary.Add($todoItem) | Out-Null
$summary.Add('') | Out-Null
$summary.Add(('Generated: {0:yyyy-MM-dd HH:mm:ss zzz}' -f (Get-Date))) | Out-Null
$summary.Add('Scope: WorldServer source (.cpp/.h), excluding ThirdParty/build output') | Out-Null
$summary.Add('Heuristic: goto statements plus standalone labels outside comments/strings, grouped to nearest enclosing function.') | Out-Null
$summary.Add(('Files scanned: {0}' -f $files.Count)) | Out-Null
$summary.Add(('Functions with goto/label flow: {0}' -f $records.Count)) | Out-Null
$summary.Add(('Goto statements: {0}' -f (($records | Measure-Object -Property goto_count -Sum).Sum))) | Out-Null
$summary.Add(('Standalone labels: {0}' -f (($records | Measure-Object -Property label_count -Sum).Sum))) | Out-Null
$summary.Add(('Files with hits: {0}' -f $fileSummary.Count)) | Out-Null
$summary.Add('') | Out-Null
$summary.Add('Top Files By Function Count') | Out-Null
foreach ($entry in ($hitSummary | Select-Object -First 20)) {
  $summary.Add(('  - {0}: {1} function(s), {2} goto(s), {3} label(s), {4} total hit(s)' -f $entry.file, $entry.functions, $entry.goto_hits, $entry.label_hits, $entry.total_hits)) | Out-Null
}
$summary.Add('') | Out-Null
$summary.Add('Top Functions By Total Hit Count') | Out-Null
foreach ($entry in ($records | Select-Object -First 20)) {
  $functionLabel = if ($entry.class -and $entry.class -ne '(unknown)') { '{0}::{1}' -f $entry.class, $entry.function } else { $entry.function }
  $summary.Add(('  - {0}:{1} {2} -> {3} hit(s) [{4}]' -f $entry.file, $entry.function_line, $functionLabel, $entry.total_hits, $entry.labels)) | Out-Null
}
$summary.Add('') | Out-Null
$summary.Add('Recommended Order') | Out-Null
$summary.Add('1. Start with contained functions that have a small label set and obvious early-return rewrites.') | Out-Null
$summary.Add('2. Then remove wider goto clusters inside the highest-hit files while keeping IDA control-flow parity.') | Out-Null
$summary.Add('3. Rescan after each cleanup pass to keep the remaining list stable.') | Out-Null
Set-Content -Path $SummaryPath -Value $summary -Encoding UTF8

Write-Host $SummaryPath
Write-Host $TsvPath
Write-Host ('records=' + $records.Count)
