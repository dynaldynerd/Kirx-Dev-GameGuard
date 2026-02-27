using MapEditor.Formats;
using OpenTK.Mathematics;

string mapDir=@"D:\Private Files\playrf\RFOnline\Map\neutralB";
string bsp=Directory.EnumerateFiles(mapDir,"*.bsp").First();
string mapName=Path.GetFileNameWithoutExtension(bsp);
string ebp=Directory.EnumerateFiles(mapDir,"*.ebp").FirstOrDefault(p=>string.Equals(Path.GetFileNameWithoutExtension(p),mapName,StringComparison.OrdinalIgnoreCase)) ?? Directory.EnumerateFiles(mapDir,"*.ebp").First();
LoadedMap map=BspLoader.Load(bsp,ebp,SkySourceMode.Sky2);
Console.WriteLine($"materials={map.Materials.Length} spans={map.BspMaterialSpans.Length} verts={map.BspRenderVertices.Length}");
int neg=0; int negVerts=0; int outRange=0; int outRangeVerts=0;
for(int i=0;i<map.BspMaterialSpans.Length;i++){
 var s=map.BspMaterialSpans[i];
 if(s.MaterialId<0){neg++; negVerts+=s.VertexCount;}
 else if(s.MaterialId>=map.Materials.Length){outRange++; outRangeVerts+=s.VertexCount;}
}
Console.WriteLine($"neg spans={neg} verts={negVerts}");
Console.WriteLine($"outRange spans={outRange} verts={outRangeVerts}");

var topNeg = map.BspMaterialSpans
 .Select((s,i)=>(s,i))
 .Where(x=>x.s.MaterialId<0)
 .OrderByDescending(x=>x.s.VertexCount)
 .Take(10)
 .ToArray();
foreach(var t in topNeg){
 int start=t.s.StartVertex; int count=t.s.VertexCount;
 Vector3 min=new(float.PositiveInfinity); Vector3 max=new(float.NegativeInfinity);
 for(int k=start;k<start+count && k<map.BspRenderVertices.Length;k++){
   var p=map.BspRenderVertices[k].Position; min=Vector3.ComponentMin(min,p); max=Vector3.ComponentMax(max,p);
 }
 Console.WriteLine($"neg span idx={t.i} start={start} count={count} light={t.s.LightMapId} bbox=({min.X:F0},{min.Y:F0},{min.Z:F0})-({max.X:F0},{max.Y:F0},{max.Z:F0})");
}
