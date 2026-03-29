using OpenTK.Mathematics;
using NumericsMatrix4 = System.Numerics.Matrix4x4;

namespace MapEditor.Formats;

public sealed class ServerMapDefinition
{
  public required int RecordIndex { get; init; }
  public required string Code { get; init; }
  public required string FileName { get; init; }
  public required int MapType { get; init; }
  public required int MapClass { get; init; }
  public required int LayerCount { get; init; }
  public required int RaceVillageCode { get; init; }
  public required int MonsterSetFileCount { get; init; }
  public required int Radius { get; init; }
  public required int LevelLimit { get; init; }
  public required int UpLevelLimit { get; init; }
  public required int PotionLimit { get; init; }
  public required int[] RacePvpUsable { get; init; }
}

public sealed class ServerHelperObject
{
  public required int HelperIndex { get; init; }
  public required string Name { get; init; }
  public required Vector3 LocalMin { get; init; }
  public required Vector3 LocalMax { get; init; }
  public required NumericsMatrix4 Transform { get; init; }
  public required NumericsMatrix4 InverseTransform { get; init; }
}

public sealed class ServerDummyPosition
{
  public required string Code { get; init; }
  public required ushort HelperIndex { get; init; }
  public required Vector3 LocalMin { get; init; }
  public required Vector3 LocalMax { get; init; }
  public required Vector3 WorldMin { get; init; }
  public required Vector3 WorldMax { get; init; }
  public required Vector3 WorldRightTop { get; init; }
  public required Vector3 WorldLeftBottom { get; init; }
  public required Vector3 WorldCenter { get; init; }
  public required Vector3 WorldDirection { get; init; }
  public required bool IsPositionable { get; init; }
  public Vector3 EditOffset { get; set; } = Vector3.Zero;
  public Vector3 EditedWorldMin => WorldMin + EditOffset;
  public Vector3 EditedWorldMax => WorldMax + EditOffset;
  public Vector3 EditedWorldRightTop => WorldRightTop + EditOffset;
  public Vector3 EditedWorldLeftBottom => WorldLeftBottom + EditOffset;
  public Vector3 EditedWorldCenter => WorldCenter + EditOffset;
  public Vector3 EditedWorldDirection => WorldDirection + EditOffset;
}

public sealed class ServerMonsterDefinition
{
  public required int RecordIndex { get; init; }
  public required string Code { get; init; }
  public required string DisplayName { get; init; }
  public required float Level { get; init; }
  public required int MobGrade { get; init; }
  public required int RaceCode { get; init; }
}

public sealed class ServerMapMonsterDummyRef
{
  public required string DummyCode { get; init; }
  public required uint SelectProp { get; init; }
  public required ServerDummyPosition Dummy { get; init; }
}

public sealed class ServerMapMonsterSpawn
{
  public required int RecordIndex { get; init; }
  public required string MonsterCode { get; init; }
  public required int MonsterRecordIndex { get; init; }
  public required string MonsterName { get; init; }
  public required float MonsterLevel { get; init; }
  public required int MonsterGrade { get; init; }
  public required int MonsterRaceCode { get; init; }
  public required uint RegenTime { get; init; }
  public required uint RegenLimit { get; init; }
  public required uint RegenProp { get; init; }
  public required uint RegenMin { get; init; }
  public required uint StandardKill { get; init; }
  public required uint RegenMax { get; init; }
}

public sealed class ServerMapMonsterSpawnSet
{
  public required int SetIndex { get; init; }
  public required ServerMapMonsterSpawn[] Spawns { get; init; }
}

public sealed class ServerMapMonsterBlock
{
  public required int RecordIndex { get; init; }
  public required string Code { get; init; }
  public required int MinCount { get; init; }
  public required int MobCount { get; init; }
  public required int MaxCount { get; init; }
  public required Vector3 WorldCenter { get; init; }
  public required ServerMapMonsterDummyRef[] Dummies { get; init; }
  public required ServerMapMonsterSpawnSet[] SpawnSets { get; init; }
}

public sealed class ServerMapPortal
{
  public required int RecordIndex { get; init; }
  public required string Code { get; init; }
  public required string LinkMapCode { get; init; }
  public required string LinkPortalCode { get; init; }
  public required string MenuText { get; init; }
  public required int NeedConCheck { get; init; }
  public required int NeedCharacterLevel { get; init; }
  public required int UpLevelLimit { get; init; }
  public required ServerDummyPosition Dummy { get; init; }
}

public sealed class ServerMapStore
{
  public required int RecordIndex { get; init; }
  public required string Code { get; init; }
  public required string BindingDummyName { get; init; }
  public required string NpcCode { get; init; }
  public required string NpcName { get; init; }
  public required string MapCode { get; init; }
  public required int TradeType { get; init; }
  public required bool SetNpcAngle { get; init; }
  public required int NpcAngle { get; init; }
  public required ServerDummyPosition Dummy { get; init; }
}

public sealed class ServerMapStartPoint
{
  public required int Index { get; init; }
  public required ServerDummyPosition Dummy { get; init; }
}

public enum ServerResourceGrade
{
  High = 0,
  Middle = 1,
  Low = 2,
}

public sealed class ServerMapResourceNode
{
  public required int Index { get; init; }
  public required ServerResourceGrade Grade { get; init; }
  public required ServerDummyPosition Dummy { get; init; }
}

public sealed class ServerMapSafeZone
{
  public required int Index { get; init; }
  public required ServerDummyPosition Dummy { get; init; }
}

public sealed class ServerMapData
{
  public required string ServerRootPath { get; init; }
  public required string MapFolderPath { get; init; }
  public required string ScriptFolderPath { get; init; }
  public required ServerMapDefinition MapDefinition { get; init; }
  public required ServerHelperObject[] Helpers { get; init; }
  public required ServerDummyPosition[] MonsterDummies { get; init; }
  public required ServerDummyPosition[] PortalDummies { get; init; }
  public required ServerDummyPosition[] StoreDummies { get; init; }
  public required ServerDummyPosition[] StartDummies { get; init; }
  public required ServerDummyPosition[] ResourceHighDummies { get; init; }
  public required ServerDummyPosition[] ResourceMiddleDummies { get; init; }
  public required ServerDummyPosition[] ResourceLowDummies { get; init; }
  public required ServerDummyPosition[] SafeDummies { get; init; }
  public required ServerMonsterDefinition[] MonsterDefinitions { get; init; }
  public required ServerMapMonsterBlock[] MonsterBlocks { get; init; }
  public required ServerMapPortal[] Portals { get; init; }
  public required ServerMapStore[] Stores { get; init; }
  public required ServerMapStartPoint[] StartPoints { get; init; }
  public required ServerMapResourceNode[] ResourceNodes { get; init; }
  public required ServerMapSafeZone[] SafeZones { get; init; }
  public required int MonsterSpawnCount { get; init; }
}
