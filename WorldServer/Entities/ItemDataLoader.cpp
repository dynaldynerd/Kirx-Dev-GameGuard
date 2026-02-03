#include "pch.h"

#include "ItemDataLoader.h"
#include "CRecordData.h"

struct ItemDataFileEntry
{
  const char *pfilename;
  int nstructsize;
};

CRecordData *s_ptblItemData = nullptr;

static const ItemDataFileEntry kItemDataFiles[] =
{
  {".\\script\\UpperItem.dat", 0x21C},
  {".\\script\\LowerItem.dat", 0x21C},
  {".\\script\\GauntletItem.dat", 0x21C},
  {".\\script\\ShoeItem.dat", 0x21C},
  {".\\script\\HelmetItem.dat", 0x21C},
  {".\\script\\ShieldItem.dat", 0x21C},
  {".\\script\\WeaponItem.dat", 0x34C},
  {".\\script\\CloakItem.dat", 0x220},
  {".\\script\\RingItem.dat", 0x1D4},
  {".\\script\\AmuletItem.dat", 0x1D4},
  {".\\script\\BulletItem.dat", 0x280},
  {".\\script\\MaketoolItem.dat", 0x1A8},
  {".\\script\\BagItem.dat", 0x188},
  {".\\script\\PotionItem.dat", 0x298},
  {".\\script\\FaceItem.dat", 0x114},
  {".\\script\\ForceItem.dat", 0x1C4},
  {".\\script\\BatteryItem.dat", 0x190},
  {".\\script\\OreItem.dat", 0x160},
  {".\\script\\ResourceItem.dat", 0x238},
  {".\\script\\UnitKeyItem.dat", 0x164},
  {".\\script\\BootyItem.dat", 0x18C},
  {".\\script\\MapItem.dat", 0x1C8},
  {".\\script\\TOWNItem.dat", 0x218},
  {".\\script\\BattleDungeonItem.dat", 0x2B0},
  {".\\script\\AnimusItem.dat", 0x188},
  {".\\script\\GuardTowerItem.dat", 0x314},
  {".\\script\\TrapItem.dat", 0x294},
  {".\\script\\SiegeKitItem.dat", 0x1F8},
  {".\\script\\TicketItem.dat", 0x210},
  {".\\script\\EventItem.dat", 0x0C8},
  {".\\script\\RecoveryItem.dat", 0x19C},
  {".\\script\\BoxItem.dat", 0x19C},
  {".\\script\\FIRECRACKER.dat", 0x19C},
  {".\\script\\UNmannedminer.dat", 0x208},
  {".\\script\\RadarItem.dat", 0x20C},
  {".\\script\\NPCLinkItem.dat", 0x1EC},
  {".\\script\\CouponItem.dat", 0x1A4},
};

char WriteTableData(int itemCount, CRecordData *itemTables, bool useHash, char *errCode)
{
  if (s_ptblItemData != nullptr)
  {
    if (errCode != nullptr)
    {
      strcpy_s(errCode, 256, "UI");
    }
    return false;
  }

  if (itemCount != 37)
  {
    if (errCode != nullptr)
    {
      sprintf_s(errCode, 256, "nItemMax<%d> != item_tbl_num<%d>", itemCount, 37);
    }
    return false;
  }

  for (int i = 0; i < 37; ++i)
  {
    if (!itemTables[i].ReadRecord(kItemDataFiles[i].pfilename, kItemDataFiles[i].nstructsize, errCode))
    {
      return false;
    }
    if (useHash && !itemTables[i].MakeHashTable(2, 5, errCode))
    {
      return false;
    }
  }

  s_ptblItemData = itemTables;
  return true;
}
