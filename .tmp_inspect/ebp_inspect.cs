using System;
using System.IO;
using System.Text;

class P
{
    struct Entry { public uint offset; public uint size; }
    static Entry ReadEntry(BinaryReader br) => new Entry{ offset=br.ReadUInt32(), size=br.ReadUInt32()};

    static void Main(string[] args)
    {
        if (args.Length < 1) { Console.WriteLine("usage: inspect <ebp>"); return; }
        string ebp = args[0];
        using var fs = File.OpenRead(ebp);
        using var br = new BinaryReader(fs);
        uint ver = br.ReadUInt32();
        if (ver != 20) { Console.WriteLine($"ver {ver}"); return; }

        Entry cfv = ReadEntry(br);
        Entry cfl = ReadEntry(br);
        Entry cflid = ReadEntry(br);
        Entry cflf = ReadEntry(br);
        Entry entityList = ReadEntry(br);
        Entry entityId = ReadEntry(br);
        Entry leafEntityList = ReadEntry(br);
        Entry soundEntityId = ReadEntry(br);
        Entry leafSoundEntityList = ReadEntry(br);
        uint spareTotal = br.ReadUInt32();
        Entry mapEntitiesList = ReadEntry(br);
        Entry soundEntityList = ReadEntry(br);
        Entry soundEntitiesList = ReadEntry(br);

        fs.Position = 0x184;
        fs.Position += (long)cfv.size + cfl.size + cflid.size + cflf.size;

        int entityDefCount = (int)(entityList.size / 0x54);
        Console.WriteLine($"entityDefCount={entityDefCount}");
        string[] names = new string[entityDefCount];
        bool[] isParticle = new bool[entityDefCount];

        for(int i=0;i<entityDefCount;i++)
        {
            byte[] nameBytes = br.ReadBytes(80);
            int nul = Array.IndexOf(nameBytes, (byte)0);
            if (nul < 0) nul = nameBytes.Length;
            string name = Encoding.ASCII.GetString(nameBytes,0,nul);
            br.ReadSingle(); // fade start
            br.ReadSingle(); // fade end
            br.ReadSingle(); // x
            br.ReadSingle(); // y
            br.ReadSingle(); // z
            br.ReadUInt16(); // type
            short shader = br.ReadInt16();
            short isP = br.ReadInt16();
            br.ReadInt16(); // mapped id
            names[i] = name;
            isParticle[i] = isP != 0;
        }

        fs.Position += entityId.size + leafEntityList.size + soundEntityId.size + leafSoundEntityList.size + spareTotal;
        int mapEntityCount = (int)(mapEntitiesList.size / 0x26);
        Console.WriteLine($"mapEntityCount={mapEntityCount}");

        var counts = new int[entityDefCount];
        for (int i=0;i<mapEntityCount;i++)
        {
            short id = br.ReadInt16();
            br.ReadSingle(); // scale
            br.ReadSingle(); br.ReadSingle(); br.ReadSingle(); // pos
            br.ReadSingle(); br.ReadSingle(); // rotx/y
            br.ReadInt16(); br.ReadInt16(); br.ReadInt16(); // bbmin
            br.ReadInt16(); br.ReadInt16(); br.ReadInt16(); // bbmax
            if (id >=0 && id < entityDefCount) counts[id]++;
        }

        for (int i=0;i<entityDefCount;i++)
        {
            if (counts[i] == 0) continue;
            string t = isParticle[i] ? "P" : "E";
            Console.WriteLine($"{i,4} {t} {counts[i],5} {names[i]}");
        }
    }
}
