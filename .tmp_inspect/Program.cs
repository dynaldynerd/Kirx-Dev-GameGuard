using System.Reflection;
const string bsp=@"D:\Private Files\playrf\RFOnline\Map\neutralB\neutralB.bsp";
const string ebp=@"D:\Private Files\playrf\RFOnline\Map\neutralB\NeutralB.ebp";
string asmPath = Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "MapEditor.dll"));
Assembly asm=Assembly.LoadFrom(asmPath);
Type loader=asm.GetType("MapEditor.Formats.BspLoader", true)!;
var load=loader.GetMethod("Load", BindingFlags.Static|BindingFlags.Public|BindingFlags.NonPublic)!;
var map=load.Invoke(null, new object[]{bsp,ebp})!;
Type t=map.GetType();
Array skyTex=(Array)t.GetProperty("SkySurfaceTextures")!.GetValue(map)!;
Type texType=skyTex.GetType().GetElementType()!;
var pName=texType.GetProperty("Name")!;
Console.WriteLine($"SkyTexCount={skyTex.Length}");
for(int i=0;i<Math.Min(6, skyTex.Length); i++){
  var tex=skyTex.GetValue(i)!;
  Console.WriteLine((string)pName.GetValue(tex)!);
}
