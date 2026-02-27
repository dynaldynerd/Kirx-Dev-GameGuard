using MapEditor.Formats;
using OpenTK.Mathematics;

string mapDir=@"D:\Private Files\playrf\RFOnline\Map\neutralB";
string bsp=Directory.EnumerateFiles(mapDir,"*.bsp").First();
string mapName=Path.GetFileNameWithoutExtension(bsp);
string ebp=Directory.EnumerateFiles(mapDir,"*.ebp").FirstOrDefault(p=>string.Equals(Path.GetFileNameWithoutExtension(p),mapName,StringComparison.OrdinalIgnoreCase)) ?? Directory.EnumerateFiles(mapDir,"*.ebp").First();
var map=BspLoader.Load(bsp,ebp,SkySourceMode.Sky2);
var pr=map.EntityScene.Models.Where(m=>m.ParticleRuntime.HasValue).OrderBy(m=>m.EntityId).ToArray();
Console.WriteLine($"ParticleRuntime models={pr.Length}");
foreach(var m in pr.Take(40)){
 int inst=map.EntityScene.Instances.Count(i=>i.EntityId==m.EntityId);
 Console.WriteLine($"id={m.EntityId} inst={inst} name={m.Name} groups={m.MatGroups.Length}");
}
Console.WriteLine("Instances of first 10 particle models:");
foreach(var m in pr.Take(10)){
 foreach(var i in map.EntityScene.Instances.Where(x=>x.EntityId==m.EntityId).Take(5)){
  Console.WriteLine($" id={m.EntityId} {m.Name} pos=({i.Position.X:F1},{i.Position.Y:F1},{i.Position.Z:F1})");
 }
}
