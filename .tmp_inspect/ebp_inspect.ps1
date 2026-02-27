param([string]$ebp)
$fs=[System.IO.File]::OpenRead($ebp)
$br=New-Object System.IO.BinaryReader($fs)
function ReadEntry($br){[pscustomobject]@{offset=$br.ReadUInt32();size=$br.ReadUInt32()}}
$ver=$br.ReadUInt32(); if($ver -ne 20){Write-Host "ver $ver"; exit}
$cfv=ReadEntry $br; $cfl=ReadEntry $br; $cflid=ReadEntry $br; $cflf=ReadEntry $br
$entityList=ReadEntry $br; $entityId=ReadEntry $br; $leafEntityList=ReadEntry $br
$soundEntityId=ReadEntry $br; $leafSoundEntityList=ReadEntry $br; $spareTotal=$br.ReadUInt32()
$mapEntitiesList=ReadEntry $br; $soundEntityList=ReadEntry $br; $soundEntitiesList=ReadEntry $br
$fs.Position=0x184 + $cfv.size + $cfl.size + $cflid.size + $cflf.size
$entityDefCount=[int]($entityList.size / 0x54)
$names=New-Object string[] $entityDefCount
$isParticle=New-Object bool[] $entityDefCount
for($i=0;$i -lt $entityDefCount;$i++){
  $nameBytes=$br.ReadBytes(80)
  $nul=[array]::IndexOf($nameBytes,[byte]0)
  if($nul -lt 0){$nul=$nameBytes.Length}
  $names[$i]=[System.Text.Encoding]::ASCII.GetString($nameBytes,0,$nul)
  $null=$br.ReadSingle();$null=$br.ReadSingle();$null=$br.ReadSingle();$null=$br.ReadSingle();$null=$br.ReadSingle();$null=$br.ReadUInt16();$null=$br.ReadInt16();
  $isParticle[$i]=($br.ReadInt16() -ne 0)
  $null=$br.ReadInt16();
}
$fs.Position += $entityId.size + $leafEntityList.size + $soundEntityId.size + $leafSoundEntityList.size + $spareTotal
$mapEntityCount=[int]($mapEntitiesList.size / 0x26)
$counts=New-Object int[] $entityDefCount
for($i=0;$i -lt $mapEntityCount;$i++){
  $id=$br.ReadInt16();
  $null=$br.ReadSingle();$null=$br.ReadSingle();$null=$br.ReadSingle();$null=$br.ReadSingle();
  $null=$br.ReadSingle();$null=$br.ReadSingle();
  $null=$br.ReadInt16();$null=$br.ReadInt16();$null=$br.ReadInt16();$null=$br.ReadInt16();$null=$br.ReadInt16();$null=$br.ReadInt16();
  if($id -ge 0 -and $id -lt $entityDefCount){$counts[$id]++}
}
for($i=0;$i -lt $entityDefCount;$i++){
  if($counts[$i] -eq 0){continue}
  $type=if($isParticle[$i]){'P'}else{'E'}
  '{0,4} {1} {2,5} {3}' -f $i,$type,$counts[$i],$names[$i]
}
$br.Close();$fs.Close();
