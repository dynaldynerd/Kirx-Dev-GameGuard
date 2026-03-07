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
  {".\\script\\UpperItem.dat", 540},
  {".\\script\\LowerItem.dat", 540},
  {".\\script\\GauntletItem.dat", 540},
  {".\\script\\ShoeItem.dat", 540},
  {".\\script\\HelmetItem.dat", 540},
  {".\\script\\ShieldItem.dat", 540},
  {".\\script\\WeaponItem.dat", 844},
  {".\\script\\CloakItem.dat", 544},
  {".\\script\\RingItem.dat", 468},
  {".\\script\\AmuletItem.dat", 468},
  {".\\script\\BulletItem.dat", 640},
  {".\\script\\MaketoolItem.dat", 424},
  {".\\script\\BagItem.dat", 392},
  {".\\script\\PotionItem.dat", 664},
  {".\\script\\FaceItem.dat", 276},
  {".\\script\\ForceItem.dat", 452},
  {".\\script\\BatteryItem.dat", 400},
  {".\\script\\OreItem.dat", 352},
  {".\\script\\ResourceItem.dat", 568},
  {".\\script\\UnitKeyItem.dat", 356},
  {".\\script\\BootyItem.dat", 396},
  {".\\script\\MapItem.dat", 456},
  {".\\script\\TOWNItem.dat", 536},
  {".\\script\\BattleDungeonItem.dat", 688},
  {".\\script\\AnimusItem.dat", 392},
  {".\\script\\GuardTowerItem.dat", 788},
  {".\\script\\TrapItem.dat", 660},
  {".\\script\\SiegeKitItem.dat", 504},
  {".\\script\\TicketItem.dat", 528},
  {".\\script\\EventItem.dat", 200},
  {".\\script\\RecoveryItem.dat", 412},
  {".\\script\\BoxItem.dat", 412},
  {".\\script\\FIRECRACKER.dat", 412},
  {".\\script\\UNmannedminer.dat", 520},
  {".\\script\\RadarItem.dat", 524},
  {".\\script\\NPCLinkItem.dat", 492},
  {".\\script\\CouponItem.dat", 420},
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
