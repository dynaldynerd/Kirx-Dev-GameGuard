namespace MapEditor.Formats;

public sealed class MapDocument
{
  private LoadedMap _map;

  private MapDocument(LoadedMap map)
  {
    _map = map ?? throw new ArgumentNullException(nameof(map));
  }

  public LoadedMap Map => _map;
  public MapEnvironmentSettings Environment => _map.Environment;
  public MaterialDefinition[] MapMaterials => _map.Materials;
  public R3TextureBlob[] MapTextures => _map.SurfaceTextures;
  public R3TextureBlob[] LightmapTextures => _map.LightmapTextures;
  public MaterialDefinition[] SkyMaterials => _map.SkyMaterials;
  public R3TextureBlob[] SkyTextures => _map.SkySurfaceTextures;
  public MaterialDefinition[] EntityMaterials => _map.EntityMaterials;
  public R3TextureBlob[] EntityTextures => _map.EntitySurfaceTextures;
  public EntitySceneData EntityScene => _map.EntityScene;
  public ServerMapData? ServerData => _map.ServerData;

  public static MapDocument FromLoadedMap(LoadedMap map)
  {
    return new MapDocument(map);
  }

  public void ReplaceMap(LoadedMap map)
  {
    _map = map ?? throw new ArgumentNullException(nameof(map));
  }

  public LoadedMap ApplyEnvironment(MapEnvironmentSettings environment)
  {
    _map = MapEditOperations.WithEnvironmentData(_map, environment);
    return _map;
  }

  public LoadedMap ApplyEntityScene(EntitySceneData entityScene)
  {
    _map = MapEditOperations.WithEntitySceneData(_map, entityScene);
    return _map;
  }

  public LoadedMap ApplyMapMaterials(MaterialDefinition[] materials)
  {
    _map = MapEditOperations.WithMapMaterialData(_map, materials);
    return _map;
  }

  public LoadedMap ApplyMapTextures(R3TextureBlob[] textures)
  {
    _map = MapEditOperations.WithMapTextureData(_map, textures);
    return _map;
  }
}
