using System.Buffers.Binary;
using MapEditor.Formats;

if (args.Length < 2)
{
  Console.WriteLine("Usage: EbpRoundTrip <bsp> <ebp>");
  return;
}

string bsp = Path.GetFullPath(args[0]);
string ebp = Path.GetFullPath(args[1]);
string outBsp = Path.Combine(Path.GetDirectoryName(ebp)!, Path.GetFileNameWithoutExtension(ebp) + ".roundtrip.bsp");
string outEbp = Path.Combine(Path.GetDirectoryName(ebp)!, Path.GetFileNameWithoutExtension(ebp) + ".roundtrip.ebp");
if (File.Exists(outBsp)) File.Delete(outBsp);
if (File.Exists(outEbp)) File.Delete(outEbp);

LoadedMap map = BspLoader.Load(bsp, ebp, SkySourceMode.Sky2);
MapExporter.ExportBspEbpPair(map, bsp, ebp, outBsp, outEbp);

byte[] src = File.ReadAllBytes(ebp);
byte[] dst = File.ReadAllBytes(outEbp);
SectionEntry[] srcEntries = ReadEntries(src);
SectionEntry[] dstEntries = ReadEntries(dst);

byte[] srcLine = ReadSection(src, srcEntries[2]);
byte[] dstLine = ReadSection(dst, dstEntries[2]);
byte[] srcLeaf = ReadSection(src, srcEntries[3]);
byte[] dstLeaf = ReadSection(dst, dstEntries[3]);

Console.WriteLine($"src={src.Length} dst={dst.Length} full_equal={src.AsSpan().SequenceEqual(dst)}");
Console.WriteLine($"CFLineId size src={srcLine.Length} dst={dstLine.Length}");
Console.WriteLine($"CFLeaf   size src={srcLeaf.Length} dst={dstLeaf.Length}");

int firstLineWord = FindFirstWordDiff(srcLine, dstLine);
int firstLeafEntry = FindFirstLeafDiff(srcLeaf, dstLeaf);
Console.WriteLine($"first CFLineId word diff={firstLineWord}");
Console.WriteLine($"first CFLeaf entry diff={firstLeafEntry}");

if (firstLineWord >= 0)
{
  DumpWordWindow("CFLineId", srcLine, dstLine, firstLineWord, 12);
}
if (firstLeafEntry >= 0)
{
  DumpLeafWindow("CFLeaf", srcLeaf, dstLeaf, firstLeafEntry, 8);
}

static SectionEntry[] ReadEntries(byte[] bytes)
{
  if (bytes.Length < 0x184) throw new InvalidDataException("header too small");
  uint version = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(0, 4));
  if (version != 20) throw new InvalidDataException($"version={version}");

  SectionEntry[] entries = new SectionEntry[48];
  int off = 4;
  for (int i = 0; i < entries.Length; ++i)
  {
    uint o = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(off, 4));
    uint s = BinaryPrimitives.ReadUInt32LittleEndian(bytes.AsSpan(off + 4, 4));
    entries[i] = new SectionEntry(o, s);
    off += 8;
  }

  return entries;
}

static byte[] ReadSection(byte[] bytes, SectionEntry entry)
{
  if (entry.Size == 0) return Array.Empty<byte>();
  if ((ulong)entry.Offset + entry.Size > (ulong)bytes.Length) return Array.Empty<byte>();
  return bytes.AsSpan((int)entry.Offset, (int)entry.Size).ToArray();
}

static int FindFirstWordDiff(byte[] a, byte[] b)
{
  int words = Math.Min(a.Length, b.Length) / 2;
  for (int i = 0; i < words; ++i)
  {
    ushort aw = BinaryPrimitives.ReadUInt16LittleEndian(a.AsSpan(i * 2, 2));
    ushort bw = BinaryPrimitives.ReadUInt16LittleEndian(b.AsSpan(i * 2, 2));
    if (aw != bw) return i;
  }

  if (a.Length != b.Length) return words;
  return -1;
}

static int FindFirstLeafDiff(byte[] a, byte[] b)
{
  int entries = Math.Min(a.Length, b.Length) / 6;
  for (int i = 0; i < entries; ++i)
  {
    int off = i * 6;
    uint asid = BinaryPrimitives.ReadUInt32LittleEndian(a.AsSpan(off, 4));
    ushort acount = BinaryPrimitives.ReadUInt16LittleEndian(a.AsSpan(off + 4, 2));
    uint bsid = BinaryPrimitives.ReadUInt32LittleEndian(b.AsSpan(off, 4));
    ushort bcount = BinaryPrimitives.ReadUInt16LittleEndian(b.AsSpan(off + 4, 2));
    if (asid != bsid || acount != bcount) return i;
  }

  if (a.Length != b.Length) return entries;
  return -1;
}

static void DumpWordWindow(string tag, byte[] a, byte[] b, int center, int radius)
{
  int words = Math.Min(a.Length, b.Length) / 2;
  int start = Math.Max(0, center - radius);
  int end = Math.Min(words - 1, center + radius);
  Console.WriteLine($"{tag} window [{start}..{end}]");
  for (int i = start; i <= end; ++i)
  {
    ushort aw = BinaryPrimitives.ReadUInt16LittleEndian(a.AsSpan(i * 2, 2));
    ushort bw = BinaryPrimitives.ReadUInt16LittleEndian(b.AsSpan(i * 2, 2));
    char mark = aw == bw ? ' ' : '*';
    Console.WriteLine($"{mark}{i,6}: src={aw,5} dst={bw,5}");
  }
}

static void DumpLeafWindow(string tag, byte[] a, byte[] b, int center, int radius)
{
  int entries = Math.Min(a.Length, b.Length) / 6;
  int start = Math.Max(0, center - radius);
  int end = Math.Min(entries - 1, center + radius);
  Console.WriteLine($"{tag} window [{start}..{end}]");
  for (int i = start; i <= end; ++i)
  {
    int off = i * 6;
    uint asid = BinaryPrimitives.ReadUInt32LittleEndian(a.AsSpan(off, 4));
    ushort acount = BinaryPrimitives.ReadUInt16LittleEndian(a.AsSpan(off + 4, 2));
    uint bsid = BinaryPrimitives.ReadUInt32LittleEndian(b.AsSpan(off, 4));
    ushort bcount = BinaryPrimitives.ReadUInt16LittleEndian(b.AsSpan(off + 4, 2));
    char mark = (asid == bsid && acount == bcount) ? ' ' : '*';
    Console.WriteLine($"{mark}{i,6}: src=({asid,6},{acount,4}) dst=({bsid,6},{bcount,4})");
  }
}

readonly record struct SectionEntry(uint Offset, uint Size);
