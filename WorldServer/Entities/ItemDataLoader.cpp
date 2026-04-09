#include "pch.h"

#include "ItemDataLoader.h"

#include "AmuletItem_fld.h"
#include "AnimusItem_fld.h"
#include "BagItem_fld.h"
#include "BattleDungeonItem_fld.h"
#include "BatteryItem_fld.h"
#include "BootyItem_fld.h"
#include "BoxItem_fld.h"
#include "BulletItem_fld.h"
#include "CRecordData.h"
#include "CloakItem_fld.h"
#include "CouponItem_fld.h"
#include "DfnEquipItem_fld.h"
#include "EventItem_fld.h"
#include "FIRECRACKER_fld.h"
#include "FaceItem_fld.h"
#include "ForceItem_fld.h"
#include "GuardTowerItem_fld.h"
#include "MakeToolItem_fld.h"
#include "MapItem_fld.h"
#include "NPCLink_fld.h"
#include "OreItem_fld.h"
#include "PotionItem_fld.h"
#include "RadarItem_fld.h"
#include "RecoveryItem_fld.h"
#include "ResourceItem_fld.h"
#include "RingItem_fld.h"
#include "SiegeKitItem_fld.h"
#include "TicketItem_fld.h"
#include "TOWNItem_fld.h"
#include "TrapItem_fld.h"
#include "UNmannedminer_fld.h"
#include "UnitKeyItem_fld.h"
#include "WeaponItem_fld.h"

struct ItemDataFileEntry
{
  const char *pfilename;
  int nstructsize;
};

CRecordData *s_ptblItemData = nullptr;

static const ItemDataFileEntry kItemDataFiles[] =
{
  {".\\script\\UpperItem.dat", static_cast<int>(sizeof(_DfnEquipItem_fld))},
  {".\\script\\LowerItem.dat", static_cast<int>(sizeof(_DfnEquipItem_fld))},
  {".\\script\\GauntletItem.dat", static_cast<int>(sizeof(_DfnEquipItem_fld))},
  {".\\script\\ShoeItem.dat", static_cast<int>(sizeof(_DfnEquipItem_fld))},
  {".\\script\\HelmetItem.dat", static_cast<int>(sizeof(_DfnEquipItem_fld))},
  {".\\script\\ShieldItem.dat", static_cast<int>(sizeof(_DfnEquipItem_fld))},
  {".\\script\\WeaponItem.dat", static_cast<int>(sizeof(_WeaponItem_fld))},
  {".\\script\\CloakItem.dat", static_cast<int>(sizeof(_CloakItem_fld))},
  {".\\script\\RingItem.dat", static_cast<int>(sizeof(_RingItem_fld))},
  {".\\script\\AmuletItem.dat", static_cast<int>(sizeof(_AmuletItem_fld))},
  {".\\script\\BulletItem.dat", static_cast<int>(sizeof(_BulletItem_fld))},
  {".\\script\\MaketoolItem.dat", static_cast<int>(sizeof(_MakeToolItem_fld))},
  {".\\script\\BagItem.dat", static_cast<int>(sizeof(_BagItem_fld))},
  {".\\script\\PotionItem.dat", static_cast<int>(sizeof(_PotionItem_fld))},
  {".\\script\\FaceItem.dat", static_cast<int>(sizeof(_FaceItem_fld))},
  {".\\script\\ForceItem.dat", static_cast<int>(sizeof(_ForceItem_fld))},
  {".\\script\\BatteryItem.dat", static_cast<int>(sizeof(_BatteryItem_fld))},
  {".\\script\\OreItem.dat", static_cast<int>(sizeof(_OreItem_fld))},
  {".\\script\\ResourceItem.dat", static_cast<int>(sizeof(_ResourceItem_fld))},
  {".\\script\\UnitKeyItem.dat", static_cast<int>(sizeof(_UnitKeyItem_fld))},
  {".\\script\\BootyItem.dat", static_cast<int>(sizeof(_BootyItem_fld))},
  {".\\script\\MapItem.dat", static_cast<int>(sizeof(_MapItem_fld))},
  {".\\script\\TOWNItem.dat", static_cast<int>(sizeof(_TOWNItem_fld))},
  {".\\script\\BattleDungeonItem.dat", static_cast<int>(sizeof(_BattleDungeonItem_fld))},
  {".\\script\\AnimusItem.dat", static_cast<int>(sizeof(_AnimusItem_fld))},
  {".\\script\\GuardTowerItem.dat", static_cast<int>(sizeof(_GuardTowerItem_fld))},
  {".\\script\\TrapItem.dat", static_cast<int>(sizeof(_TrapItem_fld))},
  {".\\script\\SiegeKitItem.dat", static_cast<int>(sizeof(_SiegeKitItem_fld))},
  {".\\script\\TicketItem.dat", static_cast<int>(sizeof(_TicketItem_fld))},
  {".\\script\\EventItem.dat", static_cast<int>(sizeof(_EventItem_fld))},
  {".\\script\\RecoveryItem.dat", static_cast<int>(sizeof(_RecoveryItem_fld))},
  {".\\script\\BoxItem.dat", static_cast<int>(sizeof(_BoxItem_fld))},
  {".\\script\\FIRECRACKER.dat", static_cast<int>(sizeof(_FIRECRACKER_fld))},
  {".\\script\\UNmannedminer.dat", static_cast<int>(sizeof(_UNmannedminer_fld))},
  {".\\script\\RadarItem.dat", static_cast<int>(sizeof(_RadarItem_fld))},
  {".\\script\\NPCLinkItem.dat", static_cast<int>(sizeof(_NPCLink_fld))},
  {".\\script\\CouponItem.dat", static_cast<int>(sizeof(_CouponItem_fld))},
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
