using MapEditor.Formats;
using OpenTK.Mathematics;

string mapDir = @"D:\Private Files\playrf\RFOnline\Map\neutralB";
string bsp = Directory.EnumerateFiles(mapDir, "*.bsp").First();
string mapName = Path.GetFileNameWithoutExtension(bsp);
string ebp = Directory.EnumerateFiles(mapDir, "*.ebp").FirstOrDefault(p => string.Equals(Path.GetFileNameWithoutExtension(p), mapName, StringComparison.OrdinalIgnoreCase)) ?? Directory.EnumerateFiles(mapDir, "*.ebp").First();
LoadedMap map = BspLoader.Load(bsp, ebp, SkySourceMode.Sky2);

List<double> areas = new(map.BspRenderVertices.Length/3);
List<(double area, int tri, Vector3 a, Vector3 b, Vector3 c)> top = new();
for(int i=0;i+2<map.BspRenderVertices.Length;i+=3)
{
  Vector3 a=map.BspRenderVertices[i].Position;
  Vector3 b=map.BspRenderVertices[i+1].Position;
  Vector3 c=map.BspRenderVertices[i+2].Position;
  double area = Vector3.Cross(b-a,c-a).Length * 0.5;
  if(double.IsFinite(area)) areas.Add(area);
  if(top.Count<20){ top.Add((area,i/3,a,b,c)); top=top.OrderByDescending(t=>t.area).ToList(); }
  else if(area>top[^1].area){ top[^1]=(area,i/3,a,b,c); top=top.OrderByDescending(t=>t.area).ToList(); }
}
areas.Sort();
double P(double p){ if(areas.Count==0)return 0; int idx=(int)Math.Clamp(Math.Round((areas.Count-1)*p),0,areas.Count-1); return areas[idx]; }
Console.WriteLine($"triangles={areas.Count}");
Console.WriteLine($"area p50={P(0.5):F3} p90={P(0.9):F3} p99={P(0.99):F3} p999={P(0.999):F3} max={areas[^1]:F3}");
Console.WriteLine("Top triangles:");
foreach(var t in top)
{
  Console.WriteLine($" tri={t.tri} area={t.area:F3} a={Fmt(t.a)} b={Fmt(t.b)} c={Fmt(t.c)}");
}
static string Fmt(Vector3 v)=>$"({v.X:F1},{v.Y:F1},{v.Z:F1})";
