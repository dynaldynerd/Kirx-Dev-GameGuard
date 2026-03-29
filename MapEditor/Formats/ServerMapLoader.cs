using System.Globalization;
using System.Text;
using OpenTK.Mathematics;
using NumericsMatrix4 = System.Numerics.Matrix4x4;
using NumericsVector3 = System.Numerics.Vector3;

namespace MapEditor.Formats;

public static class ServerMapLoader
{
  private const int MonBlockRecordSize = 1444;
  private const int MonActiveRecordSize = 92;
  private const int PortalRecordSize = 376;
  private const int StoreListRecordSize = 14284;
  private const int StoreListNameRecordSize = 772;
  private const int StoreListNameTagCount = 11;
  private const int StoreListEnglishNameTagIndex = 3;
  private const int MonsterCharacterRecordSize = 2484;
  private const int MonsterCharacterNameRecordSize = 772;
  private const int MonsterCharacterNameTagCount = 11;
  private const int MonsterCharacterEnglishNameTagIndex = 3;
  private const int MaxMonsterBlocks = 200;
  private const int MaxMonsterSetFiles = 30;
  private const int MaxMonsterActivesPerBlock = 300;
  private static readonly string[] EmptyTokenArray = Array.Empty<string>();

  public static ServerMapData Load(LoadedMap map, string loadedBspPath, string serverRootPath)
  {
    if (map == null)
    {
      throw new ArgumentNullException(nameof(map));
    }

    if (string.IsNullOrWhiteSpace(loadedBspPath))
    {
      throw new ArgumentException("Loaded BSP path is required.", nameof(loadedBspPath));
    }

    if (string.IsNullOrWhiteSpace(serverRootPath))
    {
      throw new ArgumentException("Server root path is required.", nameof(serverRootPath));
    }

    string normalizedServerRoot = NormalizeDirectoryPath(serverRootPath);
    string mapRootPath = ResolveRequiredDirectory(normalizedServerRoot, "Map");
    string scriptRootPath = ResolveRequiredDirectory(normalizedServerRoot, "Script");
    string mapDataPath = ResolveRequiredFile(mapRootPath, "Map_Data.spt");

    string loadedMapName = Path.GetFileNameWithoutExtension(loadedBspPath);
    ServerMapDefinition mapDefinition = ReadMapDefinition(mapDataPath, loadedMapName);
    ServerMapDefinition normalizedMapDefinition = NormalizeMapDefinition(mapDefinition);
    ServerMonsterDefinition[] monsterDefinitions = ReadMonsterDefinitions(scriptRootPath);
    Dictionary<string, ServerMonsterDefinition> monsterDefinitionByCode = BuildMonsterDefinitionLookup(monsterDefinitions);

    string mapFolderPath = ResolveRequiredDirectory(mapRootPath, normalizedMapDefinition.FileName);
    string helperScriptPath = ResolveRequiredFile(mapFolderPath, $"{normalizedMapDefinition.FileName}.spt");
    string[] helperTokens = TokenizeText(File.ReadAllText(helperScriptPath));
    ServerHelperObject[] helpers = ReadHelperObjects(helperTokens);
    if (helpers.Length == 0)
    {
      throw new InvalidDataException($"No helper objects were parsed from '{helperScriptPath}'.");
    }

    ServerDummyPosition[] monsterDummies = ReadDummyPositions(helperTokens, "*dm", helpers, map);
    ServerDummyPosition[] portalDummies = ReadDummyPositions(helperTokens, "*dp", helpers, map);
    ServerDummyPosition[] storeDummies = ReadDummyPositions(helperTokens, "*sd", helpers, map);
    ServerDummyPosition[] startDummies = ReadDummyPositions(helperTokens, "*ds", helpers, map);
    ServerDummyPosition[] resourceHighDummies = ReadDummyPositions(helperTokens, "*0dr", helpers, map);
    ServerDummyPosition[] resourceMiddleDummies = ReadDummyPositions(helperTokens, "*1dr", helpers, map);
    ServerDummyPosition[] resourceLowDummies = ReadDummyPositions(helperTokens, "*2dr", helpers, map);
    ServerDummyPosition[] safeDummies = ReadDummyPositions(helperTokens, "*sa", helpers, map);

    Dictionary<string, ServerDummyPosition> monsterDummyByCode = BuildDummyLookup(monsterDummies);
    Dictionary<string, ServerDummyPosition> portalDummyByCode = BuildDummyLookup(portalDummies);
    Dictionary<string, ServerDummyPosition> storeDummyByCode = BuildDummyLookup(storeDummies);

    (ServerMapMonsterBlock[] monsterBlocks, int monsterSpawnCount) = ReadMonsterBlocks(
      mapFolderPath,
      normalizedMapDefinition,
      monsterDummyByCode,
      monsterDefinitionByCode);
    ServerMapPortal[] portals = ReadPortals(mapFolderPath, scriptRootPath, normalizedMapDefinition, portalDummyByCode);
    ServerMapStore[] stores = ReadStores(scriptRootPath, normalizedMapDefinition, storeDummies, storeDummyByCode);
    ServerMapStartPoint[] startPoints = ReadStartPoints(normalizedMapDefinition, startDummies);
    ServerMapResourceNode[] resourceNodes = ReadResourceNodes(resourceHighDummies, resourceMiddleDummies, resourceLowDummies);
    ServerMapSafeZone[] safeZones = ReadSafeZones(safeDummies);

    return new ServerMapData
    {
      ServerRootPath = normalizedServerRoot,
      MapFolderPath = mapFolderPath,
      ScriptFolderPath = scriptRootPath,
      MapDefinition = normalizedMapDefinition,
      Helpers = helpers,
      MonsterDummies = monsterDummies,
      PortalDummies = portalDummies,
      StoreDummies = storeDummies,
      StartDummies = startDummies,
      ResourceHighDummies = resourceHighDummies,
      ResourceMiddleDummies = resourceMiddleDummies,
      ResourceLowDummies = resourceLowDummies,
      SafeDummies = safeDummies,
      MonsterDefinitions = monsterDefinitions,
      MonsterBlocks = monsterBlocks,
      Portals = portals,
      Stores = stores,
      StartPoints = startPoints,
      ResourceNodes = resourceNodes,
      SafeZones = safeZones,
      MonsterSpawnCount = monsterSpawnCount,
    };
  }

  private static ServerMapDefinition NormalizeMapDefinition(ServerMapDefinition definition)
  {
    int normalizedLayerCount = definition.LayerCount;
    int normalizedMonsterSetCount = definition.MonsterSetFileCount;
    if (definition.MapType == 0)
    {
      normalizedLayerCount = Math.Min(normalizedLayerCount, 1);
      normalizedMonsterSetCount = Math.Min(normalizedMonsterSetCount, 1);
    }

    return new ServerMapDefinition
    {
      RecordIndex = definition.RecordIndex,
      Code = definition.Code,
      FileName = definition.FileName,
      MapType = definition.MapType,
      MapClass = definition.MapClass,
      LayerCount = normalizedLayerCount,
      RaceVillageCode = definition.RaceVillageCode,
      MonsterSetFileCount = normalizedMonsterSetCount,
      Radius = definition.Radius,
      LevelLimit = definition.LevelLimit,
      UpLevelLimit = definition.UpLevelLimit,
      PotionLimit = definition.PotionLimit,
      RacePvpUsable = definition.RacePvpUsable,
    };
  }

  private static ServerMapDefinition ReadMapDefinition(string mapDataPath, string loadedMapName)
  {
    string[] tokens = TokenizeText(File.ReadAllText(mapDataPath));
    List<ServerMapDefinition> definitions = new();
    int recordIndex = 0;
    int tokenIndex = 0;
    while (tokenIndex < tokens.Length)
    {
      if (!string.Equals(tokens[tokenIndex], "*", StringComparison.Ordinal))
      {
        ++tokenIndex;
        continue;
      }

      if (tokenIndex + 11 >= tokens.Length)
      {
        throw new InvalidDataException($"Map_Data.spt entry {recordIndex} is truncated.");
      }

      string code = tokens[tokenIndex + 1];
      string fileName = tokens[tokenIndex + 2];
      int mapType = ParseIntToken(tokens[tokenIndex + 3], "map type");
      int mapClass = ParseIntToken(tokens[tokenIndex + 4], "map class");
      int layerCount = ParseIntToken(tokens[tokenIndex + 5], "layer count");
      int raceVillageCode = ParseIntToken(tokens[tokenIndex + 6], "race village code");
      int monsterSetCount = ParseIntToken(tokens[tokenIndex + 7], "monster set file count");
      int radius = ParseIntToken(tokens[tokenIndex + 8], "radius");
      int levelLimit = ParseIntToken(tokens[tokenIndex + 9], "level limit");
      int upLevelLimit = ParseIntToken(tokens[tokenIndex + 10], "upper level limit");
      int potionLimit = ParseIntToken(tokens[tokenIndex + 11], "potion limit");
      string racePvpFlags = tokenIndex + 12 < tokens.Length ? tokens[tokenIndex + 12] : string.Empty;

      int[] racePvpUsable = new int[3];
      for (int raceIndex = 0; raceIndex < racePvpUsable.Length && raceIndex < racePvpFlags.Length; ++raceIndex)
      {
        racePvpUsable[raceIndex] = racePvpFlags[raceIndex] - '0';
      }

      definitions.Add(new ServerMapDefinition
      {
        RecordIndex = recordIndex,
        Code = code,
        FileName = fileName,
        MapType = mapType,
        MapClass = mapClass,
        LayerCount = layerCount,
        RaceVillageCode = raceVillageCode,
        MonsterSetFileCount = monsterSetCount,
        Radius = radius,
        LevelLimit = levelLimit,
        UpLevelLimit = upLevelLimit,
        PotionLimit = potionLimit,
        RacePvpUsable = racePvpUsable,
      });

      ++recordIndex;
      tokenIndex += 13;
    }

    for (int i = 0; i < definitions.Count; ++i)
    {
      if (string.Equals(definitions[i].FileName, loadedMapName, StringComparison.OrdinalIgnoreCase))
      {
        return definitions[i];
      }
    }

    for (int i = 0; i < definitions.Count; ++i)
    {
      if (string.Equals(definitions[i].Code, loadedMapName, StringComparison.OrdinalIgnoreCase))
      {
        return definitions[i];
      }
    }

    throw new FileNotFoundException($"Map_Data.spt does not contain a record for map '{loadedMapName}'.", mapDataPath);
  }

  private static ServerHelperObject[] ReadHelperObjects(string[] tokens)
  {
    if (tokens.Length == 0)
    {
      return Array.Empty<ServerHelperObject>();
    }

    int tokenIndex = 0;
    while (tokenIndex < tokens.Length && !string.Equals(tokens[tokenIndex], "script_begin", StringComparison.OrdinalIgnoreCase))
    {
      ++tokenIndex;
    }

    if (tokenIndex >= tokens.Length)
    {
      return Array.Empty<ServerHelperObject>();
    }

    ++tokenIndex;
    List<ServerHelperObject> helpers = new();
    string? pendingToken = null;
    bool scriptEnded = false;
    while (!scriptEnded)
    {
      string token;
      if (pendingToken != null)
      {
        token = pendingToken;
        pendingToken = null;
      }
      else
      {
        if (tokenIndex >= tokens.Length)
        {
          break;
        }

        token = tokens[tokenIndex++];
      }

      if (string.Equals(token, "script_end", StringComparison.OrdinalIgnoreCase))
      {
        break;
      }

      if (token.Length == 0 || token[0] != '*')
      {
        continue;
      }

      Vector3 localMin = ReadVector3(tokens, ref tokenIndex, "helper local min");
      Vector3 localMax = ReadVector3(tokens, ref tokenIndex, "helper local max");
      NumericsMatrix4 transform = NumericsMatrix4.Identity;
      NumericsMatrix4 inverseTransform = NumericsMatrix4.Identity;

      while (tokenIndex < tokens.Length)
      {
        string command = tokens[tokenIndex++];
        if (string.Equals(command, "script_end", StringComparison.OrdinalIgnoreCase))
        {
          scriptEnded = true;
          break;
        }

        if (command.Length > 0 && command[0] == '*')
        {
          pendingToken = command;
          break;
        }

        if (string.Equals(command, "-node_tm", StringComparison.OrdinalIgnoreCase))
        {
          transform = ReadMatrix4(tokens, ref tokenIndex);
          if (!NumericsMatrix4.Invert(transform, out inverseTransform))
          {
            inverseTransform = NumericsMatrix4.Identity;
          }
          continue;
        }

        if (string.Equals(command, "-id", StringComparison.OrdinalIgnoreCase)
          || string.Equals(command, "-fog_start", StringComparison.OrdinalIgnoreCase)
          || string.Equals(command, "-fog_end", StringComparison.OrdinalIgnoreCase))
        {
          if (tokenIndex < tokens.Length)
          {
            ++tokenIndex;
          }
          continue;
        }

        if (string.Equals(command, "-fog_color", StringComparison.OrdinalIgnoreCase))
        {
          tokenIndex += Math.Min(3, tokens.Length - tokenIndex);
        }
      }

      helpers.Add(new ServerHelperObject
      {
        HelperIndex = helpers.Count,
        Name = token[1..].ToLowerInvariant(),
        LocalMin = localMin,
        LocalMax = localMax,
        Transform = transform,
        InverseTransform = inverseTransform,
      });
    }

    return helpers.ToArray();
  }

  private static ServerDummyPosition[] ReadDummyPositions(string[] tokens, string labelPrefix, IReadOnlyList<ServerHelperObject> helpers, LoadedMap map)
  {
    if (tokens.Length == 0)
    {
      return Array.Empty<ServerDummyPosition>();
    }

    List<ServerDummyPosition> dummies = new();
    int helperLineIndex = 0;
    int tokenIndex = 0;
    while (tokenIndex < tokens.Length)
    {
      string token = tokens[tokenIndex++];
      if (string.Equals(token, "[HelperObjectEnd]", StringComparison.OrdinalIgnoreCase))
      {
        break;
      }

      if (token.Length == 0 || token[0] != '*')
      {
        continue;
      }

      if (token.StartsWith(labelPrefix, StringComparison.OrdinalIgnoreCase))
      {
        if (tokenIndex + 5 >= tokens.Length)
        {
          throw new InvalidDataException($"Dummy entry '{token}' is truncated.");
        }

        Vector3 localMin = ReadIntVector3(tokens, ref tokenIndex, $"{token} local min");
        Vector3 localMax = ReadIntVector3(tokens, ref tokenIndex, $"{token} local max");
        if ((uint)helperLineIndex >= (uint)helpers.Count)
        {
          throw new InvalidDataException($"Dummy '{token}' references helper line {helperLineIndex}, but only {helpers.Count} helpers were parsed.");
        }

        ServerHelperObject helper = helpers[helperLineIndex];
        Vector3 worldMin = TransformPoint(localMin, helper.Transform);
        Vector3 worldMax = TransformPoint(localMax, helper.Transform);
        Vector3 worldRightTop = TransformPoint(new Vector3(localMax.X, localMin.Y, localMin.Z), helper.Transform);
        Vector3 worldLeftBottom = TransformPoint(new Vector3(localMin.X, localMax.Y, localMax.Z), helper.Transform);

        Vector3 localCenter = (localMin + localMax) * 0.5f;
        Vector3 worldCenter = TransformPoint(localCenter, helper.Transform);

        Vector3 localDirection = new(
          (localMax.X - localMin.X) * 0.5f + localMin.X,
          (localMax.Y - localMin.Y) * 0.5f + localMin.Y,
          localMin.Z);
        Vector3 worldDirection = TransformPoint(localDirection, helper.Transform);

        bool isPositionable = TrySampleGroundY(map, worldCenter, out float groundedY);
        if (isPositionable)
        {
          worldCenter = new Vector3(worldCenter.X, groundedY, worldCenter.Z);
          worldDirection = new Vector3(worldDirection.X, groundedY, worldDirection.Z);
        }

        dummies.Add(new ServerDummyPosition
        {
          Code = token[1..].ToLowerInvariant(),
          HelperIndex = checked((ushort)helperLineIndex),
          LocalMin = localMin,
          LocalMax = localMax,
          WorldMin = worldMin,
          WorldMax = worldMax,
          WorldRightTop = worldRightTop,
          WorldLeftBottom = worldLeftBottom,
          WorldCenter = worldCenter,
          WorldDirection = worldDirection,
          IsPositionable = isPositionable,
        });
      }

      ++helperLineIndex;
    }

    return dummies.ToArray();
  }

  private static (ServerMapMonsterBlock[] Blocks, int SpawnCount) ReadMonsterBlocks(
    string mapFolderPath,
    ServerMapDefinition mapDefinition,
    IReadOnlyDictionary<string, ServerDummyPosition> monsterDummyByCode,
    IReadOnlyDictionary<string, ServerMonsterDefinition> monsterDefinitionByCode)
  {
    string blockTablePath = Path.Combine(mapFolderPath, $"{mapDefinition.FileName}-[BLOCK].dat");
    string? resolvedBlockTablePath = FindPathCaseInsensitive(blockTablePath);
    if (resolvedBlockTablePath == null)
    {
      return (Array.Empty<ServerMapMonsterBlock>(), 0);
    }

    ServerRecord[] blockRecords = ReadFixedRecords(resolvedBlockTablePath, MonBlockRecordSize);
    if (blockRecords.Length > MaxMonsterBlocks)
    {
      throw new InvalidDataException($"Monster block count {blockRecords.Length} exceeds {MaxMonsterBlocks}.");
    }

    if (mapDefinition.MapType == 2)
    {
      return (Array.Empty<ServerMapMonsterBlock>(), 0);
    }

    if (mapDefinition.MonsterSetFileCount > MaxMonsterSetFiles)
    {
      throw new InvalidDataException($"Monster set file count {mapDefinition.MonsterSetFileCount} exceeds {MaxMonsterSetFiles}.");
    }

    List<ServerMapMonsterBlock> blocks = new(blockRecords.Length);
    int spawnCount = 0;
    for (int blockIndex = 0; blockIndex < blockRecords.Length; ++blockIndex)
    {
      ServerRecord blockRecord = blockRecords[blockIndex];
      uint dummyCount = ReadUInt32(blockRecord.Bytes, 68);
      if (dummyCount > 20)
      {
        throw new InvalidDataException($"Monster block '{blockRecord.Code}' uses {dummyCount} dummies, but only 20 dummy slots exist in the record.");
      }

      List<ServerMapMonsterDummyRef> dummyRefs = new((int)dummyCount);
      int actualDummyCount = (int)dummyCount;
      for (int dummyIndex = 0; dummyIndex < actualDummyCount; ++dummyIndex)
      {
        int dummyOffset = 72 + dummyIndex * 68;
        string dummyCode = ReadFixedString(blockRecord.Bytes, dummyOffset, 64);
        if (string.Equals(dummyCode, "0", StringComparison.Ordinal))
        {
          actualDummyCount = dummyIndex;
          break;
        }

        if (!monsterDummyByCode.TryGetValue(dummyCode, out ServerDummyPosition? dummy))
        {
          throw new InvalidDataException($"Monster block '{blockRecord.Code}' references missing dummy '{dummyCode}'.");
        }

        uint selectProp = ReadUInt32(blockRecord.Bytes, dummyOffset + 64);
        dummyRefs.Add(new ServerMapMonsterDummyRef
        {
          DummyCode = dummyCode,
          SelectProp = selectProp,
          Dummy = dummy,
        });
      }

      List<ServerMapMonsterSpawnSet> spawnSets = new(mapDefinition.MonsterSetFileCount);
      for (int setIndex = 0; setIndex < mapDefinition.MonsterSetFileCount; ++setIndex)
      {
        string setFileName = mapDefinition.MapType == 1
          ? $"{blockRecord.Code}_{setIndex}.dat"
          : $"{blockRecord.Code}.dat";
        string setPath = ResolveRequiredFile(mapFolderPath, setFileName);
        ServerRecord[] activeRecords = ReadFixedRecords(setPath, MonActiveRecordSize);
        if (activeRecords.Length > MaxMonsterActivesPerBlock)
        {
          throw new InvalidDataException($"Monster block '{blockRecord.Code}' set {setIndex} contains {activeRecords.Length} spawn records, exceeding {MaxMonsterActivesPerBlock}.");
        }

        ServerMapMonsterSpawn[] spawns = new ServerMapMonsterSpawn[activeRecords.Length];
        for (int activeIndex = 0; activeIndex < activeRecords.Length; ++activeIndex)
        {
          ServerRecord activeRecord = activeRecords[activeIndex];
          if (!monsterDefinitionByCode.TryGetValue(activeRecord.Code, out ServerMonsterDefinition? monsterDefinition))
          {
            throw new InvalidDataException($"Monster block '{blockRecord.Code}' references missing monster '{activeRecord.Code}' in MonsterCharacter.dat.");
          }

          spawns[activeIndex] = new ServerMapMonsterSpawn
          {
            RecordIndex = activeRecord.Index,
            MonsterCode = activeRecord.Code,
            MonsterRecordIndex = monsterDefinition.RecordIndex,
            MonsterName = monsterDefinition.DisplayName,
            MonsterLevel = monsterDefinition.Level,
            MonsterGrade = monsterDefinition.MobGrade,
            MonsterRaceCode = monsterDefinition.RaceCode,
            RegenTime = ReadUInt32(activeRecord.Bytes, 68),
            RegenLimit = ReadUInt32(activeRecord.Bytes, 72),
            RegenProp = ReadUInt32(activeRecord.Bytes, 76),
            RegenMin = ReadUInt32(activeRecord.Bytes, 80),
            StandardKill = ReadUInt32(activeRecord.Bytes, 84),
            RegenMax = ReadUInt32(activeRecord.Bytes, 88),
          };
        }

        spawnCount += spawns.Length;
        spawnSets.Add(new ServerMapMonsterSpawnSet
        {
          SetIndex = setIndex,
          Spawns = spawns,
        });
      }

      blocks.Add(new ServerMapMonsterBlock
      {
        RecordIndex = blockRecord.Index,
        Code = blockRecord.Code,
        MinCount = ReadInt32(blockRecord.Bytes, 1432),
        MobCount = ReadInt32(blockRecord.Bytes, 1436),
        MaxCount = ReadInt32(blockRecord.Bytes, 1440),
        WorldCenter = ComputeAverageDummyCenter(dummyRefs),
        Dummies = dummyRefs.ToArray(),
        SpawnSets = spawnSets.ToArray(),
      });
    }

    return (blocks.ToArray(), spawnCount);
  }

  private static ServerMapPortal[] ReadPortals(
    string mapFolderPath,
    string scriptRootPath,
    ServerMapDefinition mapDefinition,
    IReadOnlyDictionary<string, ServerDummyPosition> portalDummyByCode)
  {
    string portalPath = ResolveRequiredFile(
      mapFolderPath,
      scriptRootPath,
      $"{mapDefinition.FileName}-[Portal].dat",
      $"{mapDefinition.FileName}-Portal.dat",
      $"{mapDefinition.FileName}-PORTAL.dat");
    ServerRecord[] portalRecords = ReadFixedRecords(portalPath, PortalRecordSize);
    ServerMapPortal[] portals = new ServerMapPortal[portalRecords.Length];
    for (int portalIndex = 0; portalIndex < portalRecords.Length; ++portalIndex)
    {
      ServerRecord record = portalRecords[portalIndex];
      if (!portalDummyByCode.TryGetValue(record.Code, out ServerDummyPosition? dummy))
      {
        throw new InvalidDataException($"Portal '{record.Code}' is missing its portal dummy.");
      }

      portals[portalIndex] = new ServerMapPortal
      {
        RecordIndex = record.Index,
        Code = record.Code,
        LinkMapCode = ReadFixedString(record.Bytes, 68, 64),
        LinkPortalCode = ReadFixedString(record.Bytes, 132, 64),
        MenuText = ReadFixedString(record.Bytes, 196, 64),
        NeedConCheck = ReadInt32(record.Bytes, 260),
        NeedCharacterLevel = ReadInt32(record.Bytes, 264),
        UpLevelLimit = ReadInt32(record.Bytes, 268),
        Dummy = dummy,
      };
    }

    return portals;
  }

  private static ServerMapStore[] ReadStores(
    string scriptRootPath,
    ServerMapDefinition mapDefinition,
    ServerDummyPosition[] storeDummies,
    IReadOnlyDictionary<string, ServerDummyPosition> storeDummyByCode)
  {
    if (storeDummies.Length == 0)
    {
      return Array.Empty<ServerMapStore>();
    }

    string storeListPath = ResolveRequiredFile(scriptRootPath, "StoreList.dat");
    string storeListNamePath = ResolveRequiredFile(scriptRootPath, "StoreList_str.dat");
    ServerRecord[] storeRecords = ReadFixedRecords(storeListPath, StoreListRecordSize);
    Dictionary<string, string> displayNameByCode = ReadDisplayNameLookup(
      storeListNamePath,
      StoreListNameRecordSize,
      StoreListNameTagCount,
      StoreListEnglishNameTagIndex);
    List<ServerMapStore> stores = new();
    for (int recordIndex = 0; recordIndex < storeRecords.Length; ++recordIndex)
    {
      ServerRecord record = storeRecords[recordIndex];
      string storeMapCode = ReadFixedString(record.Bytes, 260, 64);
      if (!string.Equals(storeMapCode, mapDefinition.FileName, StringComparison.OrdinalIgnoreCase))
      {
        continue;
      }

      if (!storeDummyByCode.TryGetValue(record.Code, out ServerDummyPosition? dummy))
      {
        throw new InvalidDataException($"Store '{record.Code}' on map '{mapDefinition.FileName}' is missing its store dummy.");
      }

      string rawName = ReadFixedString(record.Bytes, 196, 64);
      string displayName =
        displayNameByCode.TryGetValue(record.Code, out string? localizedName) && !string.IsNullOrWhiteSpace(localizedName)
          ? localizedName
          : rawName;
      stores.Add(new ServerMapStore
      {
        RecordIndex = record.Index,
        Code = record.Code,
        BindingDummyName = ReadFixedString(record.Bytes, 68, 64),
        NpcCode = ReadFixedString(record.Bytes, 132, 64),
        NpcName = !string.IsNullOrWhiteSpace(displayName) ? displayName : record.Code,
        MapCode = storeMapCode,
        TradeType = ReadInt32(record.Bytes, 324),
        SetNpcAngle = ReadInt32(record.Bytes, 328) != 0,
        NpcAngle = ReadInt32(record.Bytes, 332),
        Dummy = dummy,
      });
    }

    return stores.ToArray();
  }

  private static ServerMapStartPoint[] ReadStartPoints(ServerMapDefinition mapDefinition, ServerDummyPosition[] startDummies)
  {
    int startCount = startDummies.Length;
    if (mapDefinition.MapClass == 1 && startCount < 3)
    {
      throw new InvalidDataException($"Map '{mapDefinition.FileName}' requires at least 3 start dummies for map class 1.");
    }

    if (mapDefinition.RaceVillageCode < 3 && startCount == 0)
    {
      throw new InvalidDataException($"Map '{mapDefinition.FileName}' requires at least 1 start dummy for race village code {mapDefinition.RaceVillageCode}.");
    }

    if (mapDefinition.MapType == 2 && startCount == 0)
    {
      throw new InvalidDataException($"Map '{mapDefinition.FileName}' requires at least 1 start dummy for map type 2.");
    }

    ServerMapStartPoint[] startPoints = new ServerMapStartPoint[startCount];
    for (int index = 0; index < startCount; ++index)
    {
      startPoints[index] = new ServerMapStartPoint
      {
        Index = index,
        Dummy = startDummies[index],
      };
    }

    return startPoints;
  }

  private static ServerMapResourceNode[] ReadResourceNodes(
    ServerDummyPosition[] high,
    ServerDummyPosition[] middle,
    ServerDummyPosition[] low)
  {
    List<ServerMapResourceNode> resources = new(high.Length + middle.Length + low.Length);
    AppendResourceNodes(resources, high, ServerResourceGrade.High);
    AppendResourceNodes(resources, middle, ServerResourceGrade.Middle);
    AppendResourceNodes(resources, low, ServerResourceGrade.Low);
    return resources.ToArray();
  }

  private static void AppendResourceNodes(List<ServerMapResourceNode> output, ServerDummyPosition[] source, ServerResourceGrade grade)
  {
    for (int index = 0; index < source.Length; ++index)
    {
      output.Add(new ServerMapResourceNode
      {
        Index = output.Count,
        Grade = grade,
        Dummy = source[index],
      });
    }
  }

  private static ServerMapSafeZone[] ReadSafeZones(ServerDummyPosition[] safeDummies)
  {
    ServerMapSafeZone[] safeZones = new ServerMapSafeZone[safeDummies.Length];
    for (int index = 0; index < safeDummies.Length; ++index)
    {
      safeZones[index] = new ServerMapSafeZone
      {
        Index = index,
        Dummy = safeDummies[index],
      };
    }

    return safeZones;
  }

  private static Dictionary<string, string> ReadDisplayNameLookup(
    string path,
    int recordSize,
    int tagCount,
    int preferredTagIndex)
  {
    ServerRecord[] records = ReadFixedRecords(path, recordSize);
    Dictionary<string, string> displayNameByCode = new(records.Length, StringComparer.OrdinalIgnoreCase);
    for (int index = 0; index < records.Length; ++index)
    {
      ServerRecord record = records[index];
      string displayName = ReadDisplayName(record.Bytes, 68, tagCount, preferredTagIndex);
      if (!string.IsNullOrWhiteSpace(displayName))
      {
        displayNameByCode[record.Code] = displayName;
      }
    }

    return displayNameByCode;
  }

  private static ServerMonsterDefinition[] ReadMonsterDefinitions(string scriptRootPath)
  {
    string monsterPath = ResolveRequiredFile(scriptRootPath, "MonsterCharacter.dat");
    string monsterNamePath = ResolveRequiredFile(scriptRootPath, "MonsterCharacter_str.dat");
    ServerRecord[] monsterRecords = ReadFixedRecords(monsterPath, MonsterCharacterRecordSize);
    Dictionary<string, string> displayNameByCode = ReadDisplayNameLookup(
      monsterNamePath,
      MonsterCharacterNameRecordSize,
      MonsterCharacterNameTagCount,
      MonsterCharacterEnglishNameTagIndex);

    ServerMonsterDefinition[] monsterDefinitions = new ServerMonsterDefinition[monsterRecords.Length];
    for (int index = 0; index < monsterRecords.Length; ++index)
    {
      ServerRecord monsterRecord = monsterRecords[index];
      string rawName = ReadFixedString(monsterRecord.Bytes, 68, 64);
      string displayName =
        displayNameByCode.TryGetValue(monsterRecord.Code, out string? localizedName) && !string.IsNullOrWhiteSpace(localizedName)
          ? localizedName
          : (!string.IsNullOrWhiteSpace(rawName) ? rawName : monsterRecord.Code);

      monsterDefinitions[index] = new ServerMonsterDefinition
      {
        RecordIndex = monsterRecord.Index,
        Code = monsterRecord.Code,
        DisplayName = displayName,
        Level = ReadSingle(monsterRecord.Bytes, 268),
        MobGrade = ReadInt32(monsterRecord.Bytes, 196),
        RaceCode = ReadInt32(monsterRecord.Bytes, 200),
      };
    }

    return monsterDefinitions;
  }

  private static Dictionary<string, ServerDummyPosition> BuildDummyLookup(ServerDummyPosition[] dummies)
  {
    Dictionary<string, ServerDummyPosition> lookup = new(StringComparer.OrdinalIgnoreCase);
    for (int index = 0; index < dummies.Length; ++index)
    {
      lookup[dummies[index].Code] = dummies[index];
    }

    return lookup;
  }

  private static Dictionary<string, ServerMonsterDefinition> BuildMonsterDefinitionLookup(ServerMonsterDefinition[] monsterDefinitions)
  {
    Dictionary<string, ServerMonsterDefinition> lookup = new(StringComparer.OrdinalIgnoreCase);
    for (int index = 0; index < monsterDefinitions.Length; ++index)
    {
      lookup[monsterDefinitions[index].Code] = monsterDefinitions[index];
    }

    return lookup;
  }

  private static Vector3 ComputeAverageDummyCenter(IReadOnlyList<ServerMapMonsterDummyRef> dummyRefs)
  {
    if (dummyRefs.Count == 0)
    {
      return Vector3.Zero;
    }

    Vector3 sum = Vector3.Zero;
    for (int index = 0; index < dummyRefs.Count; ++index)
    {
      sum += dummyRefs[index].Dummy.WorldCenter;
    }

    return sum / dummyRefs.Count;
  }

  private static bool TrySampleGroundY(LoadedMap map, Vector3 point, out float groundedY)
  {
    groundedY = point.Y;
    Vector3[] triangles = map.BspTriangleVertices;
    if (triangles == null || triangles.Length < 3)
    {
      return false;
    }

    float rayOriginY = MathF.Max(map.Bounds.Max.Y, point.Y) + 5000.0f;
    Vector3 rayOrigin = new(point.X, rayOriginY, point.Z);
    Vector3 rayDirection = -Vector3.UnitY;
    float bestT = float.PositiveInfinity;
    bool found = false;
    for (int index = 0; index + 2 < triangles.Length; index += 3)
    {
      Vector3 v0 = triangles[index];
      Vector3 v1 = triangles[index + 1];
      Vector3 v2 = triangles[index + 2];

      float minX = MathF.Min(v0.X, MathF.Min(v1.X, v2.X)) - 0.001f;
      float maxX = MathF.Max(v0.X, MathF.Max(v1.X, v2.X)) + 0.001f;
      float minZ = MathF.Min(v0.Z, MathF.Min(v1.Z, v2.Z)) - 0.001f;
      float maxZ = MathF.Max(v0.Z, MathF.Max(v1.Z, v2.Z)) + 0.001f;
      if (point.X < minX || point.X > maxX || point.Z < minZ || point.Z > maxZ)
      {
        continue;
      }

      if (!TryIntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, out float t))
      {
        continue;
      }

      if (t < bestT)
      {
        bestT = t;
        found = true;
      }
    }

    if (!found)
    {
      return false;
    }

    groundedY = rayOrigin.Y - bestT;
    return float.IsFinite(groundedY);
  }

  private static bool TryIntersectRayTriangle(
    Vector3 rayOrigin,
    Vector3 rayDirection,
    Vector3 v0,
    Vector3 v1,
    Vector3 v2,
    out float t)
  {
    t = 0.0f;
    const float epsilon = 0.000001f;

    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 pvec = Vector3.Cross(rayDirection, edge2);
    float det = Vector3.Dot(edge1, pvec);
    if (det > -epsilon && det < epsilon)
    {
      return false;
    }

    float invDet = 1.0f / det;
    Vector3 tvec = rayOrigin - v0;
    float u = Vector3.Dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
    {
      return false;
    }

    Vector3 qvec = Vector3.Cross(tvec, edge1);
    float v = Vector3.Dot(rayDirection, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
    {
      return false;
    }

    t = Vector3.Dot(edge2, qvec) * invDet;
    return t >= 0.0f;
  }

  private static ServerRecord[] ReadFixedRecords(string path, int expectedRecordSize)
  {
    using FileStream stream = File.OpenRead(path);
    using BinaryReader reader = new(stream);

    if (stream.Length < 12)
    {
      throw new InvalidDataException($"Record table '{path}' is too small.");
    }

    int recordCount = reader.ReadInt32();
    int fieldCount = reader.ReadInt32();
    int recordSize = reader.ReadInt32();
    if (recordCount < 0 || fieldCount < 0 || recordSize <= 0)
    {
      throw new InvalidDataException($"Record table '{path}' has an invalid header.");
    }

    if (recordSize != expectedRecordSize)
    {
      throw new InvalidDataException($"Record table '{path}' has record size {recordSize}, expected {expectedRecordSize}.");
    }

    long expectedSize = 12L + (long)recordCount * recordSize;
    if (stream.Length != expectedSize)
    {
      throw new InvalidDataException($"Record table '{path}' has invalid file size {stream.Length}; expected {expectedSize}.");
    }

    ServerRecord[] records = new ServerRecord[recordCount];
    for (int recordIndex = 0; recordIndex < recordCount; ++recordIndex)
    {
      byte[] bytes = reader.ReadBytes(recordSize);
      if (bytes.Length != recordSize)
      {
        throw new EndOfStreamException($"Unexpected EOF while reading '{path}' record {recordIndex}.");
      }

      records[recordIndex] = new ServerRecord(
        recordIndex,
        ReadUInt32(bytes, 0),
        ReadFixedString(bytes, 4, 64),
        bytes);
    }

    return records;
  }

  private static string[] TokenizeText(string text)
  {
    return string.IsNullOrWhiteSpace(text)
      ? EmptyTokenArray
      : text.Split((char[]?)null, StringSplitOptions.RemoveEmptyEntries);
  }

  private static Vector3 ReadVector3(string[] tokens, ref int tokenIndex, string fieldName)
  {
    return new Vector3(
      ReadFloatToken(tokens, ref tokenIndex, fieldName),
      ReadFloatToken(tokens, ref tokenIndex, fieldName),
      ReadFloatToken(tokens, ref tokenIndex, fieldName));
  }

  private static Vector3 ReadIntVector3(string[] tokens, ref int tokenIndex, string fieldName)
  {
    return new Vector3(
      ReadIntToken(tokens, ref tokenIndex, fieldName),
      ReadIntToken(tokens, ref tokenIndex, fieldName),
      ReadIntToken(tokens, ref tokenIndex, fieldName));
  }

  private static NumericsMatrix4 ReadMatrix4(string[] tokens, ref int tokenIndex)
  {
    Span<float> values = stackalloc float[16];
    for (int index = 0; index < values.Length; ++index)
    {
      values[index] = ReadFloatToken(tokens, ref tokenIndex, "matrix");
    }

    return new NumericsMatrix4(
      values[0], values[1], values[2], values[3],
      values[4], values[5], values[6], values[7],
      values[8], values[9], values[10], values[11],
      values[12], values[13], values[14], values[15]);
  }

  private static int ReadIntToken(string[] tokens, ref int tokenIndex, string fieldName)
  {
    if (tokenIndex >= tokens.Length)
    {
      throw new InvalidDataException($"Missing integer token for {fieldName}.");
    }

    int value = ParseIntToken(tokens[tokenIndex], fieldName);
    ++tokenIndex;
    return value;
  }

  private static float ReadFloatToken(string[] tokens, ref int tokenIndex, string fieldName)
  {
    if (tokenIndex >= tokens.Length)
    {
      throw new InvalidDataException($"Missing float token for {fieldName}.");
    }

    string token = tokens[tokenIndex++];
    if (!float.TryParse(token, NumberStyles.Float | NumberStyles.AllowThousands, CultureInfo.InvariantCulture, out float value))
    {
      throw new InvalidDataException($"Invalid float token '{token}' for {fieldName}.");
    }

    return value;
  }

  private static int ParseIntToken(string token, string fieldName)
  {
    if (!int.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out int value))
    {
      throw new InvalidDataException($"Invalid integer token '{token}' for {fieldName}.");
    }

    return value;
  }

  private static Vector3 TransformPoint(Vector3 source, in NumericsMatrix4 matrix)
  {
    NumericsVector3 transformed = NumericsVector3.Transform(new NumericsVector3(source.X, source.Y, source.Z), matrix);
    return new Vector3(transformed.X, transformed.Y, transformed.Z);
  }

  private static string ResolveRequiredDirectory(string rootPath, string childName)
  {
    string? resolved = FindPathCaseInsensitive(Path.Combine(rootPath, childName));
    if (resolved == null || !Directory.Exists(resolved))
    {
      throw new DirectoryNotFoundException($"Required directory '{childName}' was not found under '{rootPath}'.");
    }

    return NormalizeDirectoryPath(resolved);
  }

  private static string ResolveRequiredFile(string rootPath, string childName)
  {
    string? resolved = FindPathCaseInsensitive(Path.Combine(rootPath, childName));
    if (resolved == null || !File.Exists(resolved))
    {
      throw new FileNotFoundException($"Required file '{childName}' was not found under '{rootPath}'.", Path.Combine(rootPath, childName));
    }

    return Path.GetFullPath(resolved);
  }

  private static string ResolveRequiredFile(string firstRootPath, string secondRootPath, params string[] childNames)
  {
    for (int rootIndex = 0; rootIndex < 2; ++rootIndex)
    {
      string rootPath = rootIndex == 0 ? firstRootPath : secondRootPath;
      for (int childIndex = 0; childIndex < childNames.Length; ++childIndex)
      {
        string? resolved = FindPathCaseInsensitive(Path.Combine(rootPath, childNames[childIndex]));
        if (resolved != null && File.Exists(resolved))
        {
          return Path.GetFullPath(resolved);
        }
      }
    }

    throw new FileNotFoundException(
      $"Required file was not found in '{firstRootPath}' or '{secondRootPath}'. Tried: {string.Join(", ", childNames)}");
  }

  private static string NormalizeDirectoryPath(string path)
  {
    return Path.GetFullPath(path).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
  }

  private static string? FindPathCaseInsensitive(string path)
  {
    string fullPath = Path.GetFullPath(path);
    if (File.Exists(fullPath) || Directory.Exists(fullPath))
    {
      return fullPath;
    }

    string root = Path.GetPathRoot(fullPath) ?? string.Empty;
    if (string.IsNullOrWhiteSpace(root) || !Directory.Exists(root))
    {
      return null;
    }

    string current = root;
    string remainder = fullPath[root.Length..];
    string[] segments = remainder.Split([Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar], StringSplitOptions.RemoveEmptyEntries);
    for (int index = 0; index < segments.Length; ++index)
    {
      string segment = segments[index];
      string? match = Directory
        .EnumerateFileSystemEntries(current)
        .FirstOrDefault(entry => string.Equals(Path.GetFileName(entry), segment, StringComparison.OrdinalIgnoreCase));
      if (match == null)
      {
        return null;
      }

      current = match;
    }

    return current;
  }

  private static uint ReadUInt32(byte[] bytes, int offset)
  {
    return BitConverter.ToUInt32(bytes, offset);
  }

  private static int ReadInt32(byte[] bytes, int offset)
  {
    return BitConverter.ToInt32(bytes, offset);
  }

  private static float ReadSingle(byte[] bytes, int offset)
  {
    return BitConverter.ToSingle(bytes, offset);
  }

  private static string ReadFixedString(byte[] bytes, int offset, int length)
  {
    int end = offset;
    int max = Math.Min(bytes.Length, offset + length);
    while (end < max && bytes[end] != 0)
    {
      ++end;
    }

    return Encoding.ASCII.GetString(bytes, offset, end - offset).Trim();
  }

  private static string ReadDisplayName(byte[] bytes, int offset, int tagCount, int preferredTagIndex)
  {
    if ((uint)preferredTagIndex < (uint)tagCount)
    {
      string preferred = ReadFixedString(bytes, offset + preferredTagIndex * 64, 64);
      if (!string.IsNullOrWhiteSpace(preferred))
      {
        return preferred;
      }
    }

    for (int tagIndex = 0; tagIndex < tagCount; ++tagIndex)
    {
      string tag = ReadFixedString(bytes, offset + tagIndex * 64, 64);
      if (!string.IsNullOrWhiteSpace(tag))
      {
        return tag;
      }
    }

    return string.Empty;
  }

  private readonly record struct ServerRecord(int Index, uint DeclaredIndex, string Code, byte[] Bytes);
}
