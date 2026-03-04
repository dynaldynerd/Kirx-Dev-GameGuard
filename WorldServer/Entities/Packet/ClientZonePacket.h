#pragma once

#include "../IdaCompat.h"
#include "../StoragePos.h"

// Client -> Zone packet structs.

// a_trade_adjust_price_request_clzo.h
#pragma pack(push, 1)
struct _a_trade_adjust_price_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwNewPrice;
  unsigned int dwRegistSerial;
  unsigned __int8 byTaxRate;
};
#pragma pack(pop)

// a_trade_clear_item_request_clzo.h
#pragma pack(push, 1)
struct _a_trade_clear_item_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned int dwRegistSerial;
};
#pragma pack(pop)

// a_trade_reg_item_request_clzo.h
#pragma pack(push, 1)
struct _a_trade_reg_item_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int16 wItemSerial;
  unsigned __int8 byAmount;
  unsigned __int8 byItemTableCode;
  unsigned __int16 wItemIndex;
  unsigned int dwPrice;
  unsigned __int8 byTax;
  int bUseNpcLink;
};
#pragma pack(pop)

// alter_item_slot_request_clzo.h
#pragma pack(push, 1)
struct _alter_item_slot_request_clzo
{
  struct __list
  {
    unsigned __int8 byStorageIndex;
    unsigned int dwItemSerial;
    unsigned __int8 byClientSlotIndex;
  };
  unsigned __int8 byNum;
  __list list[100];
};
#pragma pack(pop)

// alter_link_slot_request_clzo.h
#pragma pack(push, 1)
struct _alter_link_slot_request_clzo
{
  struct __list
  {
    unsigned __int8 bySlotIndex;
    unsigned __int8 byLinkCode;
    unsigned __int16 wIndex;
  };
  unsigned __int8 byLinkLock;
  unsigned __int8 byNum;
  __list list[50];
};
#pragma pack(pop)

// alter_party_loot_share_request_clzo.h
#pragma pack(push, 1)
struct _alter_party_loot_share_request_clzo
{
  unsigned __int8 byLootShareMode;
};
#pragma pack(pop)

// buy_store_request_clzo.h
#pragma pack(push, 1)
struct _buy_store_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned int dwGoodSerial;
    unsigned __int8 byAmount;
  };
  unsigned int dwStoreIndex;
  unsigned __int8 byBuyNum;
  int bUseNPCLinkIntem;
  _list OfferList[100];
};
#pragma pack(pop)

// combine_ex_item_accept_request_clzo.h
#pragma pack(push, 2)
#pragma pack(push, 1)
struct _combine_ex_item_accept_request_clzo
{
  struct _Select_ItemList_buff
  {
    unsigned __int8 bySelectNum;
    unsigned __int8 bySelectIndexList[24];
  };
  unsigned __int8 byDlgType;
  unsigned int dwCheckKey;
  _Select_ItemList_buff SelectItemBuff;
};
#pragma pack(pop)
#pragma pack(pop)

// combine_ex_item_request_clzo.h
#pragma pack(push, 1)
struct _combine_ex_item_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned __int16 wItemSerial;
    unsigned __int8 byAmount;
  };
  unsigned int wManualIndex;
  unsigned __int8 byCombineSlotNum;
  _list iCombineSlot[5];
  int bUseNpcLink;
};
#pragma pack(pop)

// combine_item_request_clzo.h
#pragma pack(push, 1)
struct _combine_item_request_clzo
{
  unsigned __int16 wManualIndex;
  unsigned __int16 wOverlapItemSerial;
  unsigned __int8 byMaterialNum;
  _STORAGE_POS_INDIV ipMaterials[100];
};
#pragma pack(pop)

// guild_honor_set_request_clzo.h
#pragma pack(push, 1)
struct _guild_honor_set_request_clzo
{
  struct __list
  {
    char wszGuildName[17];
    unsigned __int8 byTaxRate;
  };
  unsigned __int8 byListNum;
  __list GuildList[5];
};
#pragma pack(pop)

// guild_manage_request_clzo.h
#pragma pack(push, 1)
struct _guild_manage_request_clzo
{
  _guild_manage_request_clzo();
  __int64 size();
  unsigned __int8 byManageType;
  unsigned int dwManageDst;
  unsigned int dwManageObj1;
  unsigned int dwManageObj2;
  unsigned int dwManageObj3;
};
#pragma pack(pop)

// guildroom_enter_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_enter_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

// guildroom_out_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_out_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

// guildroom_rent_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_rent_request_clzo
{
  unsigned int dwGuildSerial;
  unsigned __int8 byRoomType;
};
#pragma pack(pop)

// guildroom_resttime_request_clzo.h
#pragma pack(push, 1)
struct _guildroom_resttime_request_clzo
{
  unsigned int dwGuildSerial;
};
#pragma pack(pop)

// make_item_request_clzo.h
#pragma pack(push, 1)
struct _make_item_request_clzo
{
  _STORAGE_POS_INDIV ipMakeTool;
  unsigned __int16 wManualIndex;
  unsigned __int8 byMaterialNum;
  _STORAGE_POS_INDIV ipMaterials[100];
};
#pragma pack(pop)

// make_tower_request_clzo.h
#pragma pack(push, 1)
struct _make_tower_request_clzo
{
  struct __material
  {
    unsigned __int16 wItemSerial;
    unsigned __int8 byMaterSlotIndex;
    unsigned __int8 byAmount;
  };
  unsigned __int16 wSkillIndex;
  __int16 zPos[3];
  unsigned __int16 wTowerItemSerial;
  unsigned __int16 wConsumeItemSerial[3];
  unsigned __int8 byMaterialNum;
  __material Material[30];
};
#pragma pack(pop)

// other_shape_request_clzo.h
#pragma pack(push, 1)
struct _other_shape_request_clzo
{
  enum : unsigned __int8
  {
    SHAPE_ALL = 0,
    SHAPE_PART = 1
  };
  unsigned __int16 wIndex;
  unsigned __int8 byReqType;
  unsigned __int8 byStateFlag;
};
#pragma pack(pop)

// request_csi_buy_clzo.h
#pragma pack(push, 1)
struct _request_csi_buy_clzo
{
  struct __item
  {
    int nPrice;
    unsigned __int8 byDiscount;
    unsigned __int8 byTblCode;
    unsigned __int16 wItemIdx;
    unsigned __int16 wStoreIdx;
    unsigned __int8 byOverlapNum;
    int nEvent[8];
    unsigned __int8 byEventType;
  };
  unsigned __int8 nNum;
  unsigned __int8 bySetKind;
  unsigned __int8 byCouponNum;
  _STORAGE_POS_INDIV CouponItem[3];
  __item item[20];
};
#pragma pack(pop)

// sell_store_request_clzo.h
#pragma pack(push, 1)
struct _sell_store_request_clzo
{
  struct _list
  {
    unsigned __int8 byStorageCode;
    unsigned __int16 wSerial;
    unsigned __int8 byAmount;
  };
  unsigned int dwStoreIndex;
  unsigned __int8 bySellNum;
  int bUseNPCLinkIntem;
  _list Item[100];
};
#pragma pack(pop)

// talik_crystal_exchange_clzo.h
#pragma pack(push, 1)
struct _talik_crystal_exchange_clzo
{
  struct _list
  {
    unsigned __int16 wSerial;
    unsigned __int8 byItemCount;
  };
  unsigned __int8 byExchangeNum;
  _list Item[24];
};
#pragma pack(pop)

// unit_pack_fill_request_clzo.h
#pragma pack(push, 1)
struct _unit_pack_fill_request_clzo
{
  struct __list
  {
    unsigned __int8 bySpareIndex;
    unsigned __int16 wBulletIndex;
  };
  unsigned __int8 bySlotIndex;
  unsigned __int8 byFillNum;
  int bUseNPCLinkIntem;
  __list List[8];
};
#pragma pack(pop)

// unmannedtrader_buy_item_request_clzo.h
#pragma pack(push, 1)
struct _unmannedtrader_buy_item_request_clzo
{
  struct __list
  {
    unsigned int dwRegistSerial;
    unsigned int dwPrice;
  };
  unsigned __int8 byDivision;
  unsigned __int8 byClass;
  unsigned __int8 bySubClass;
  unsigned int dwVer;
  unsigned __int8 byStoreIndex;
  unsigned __int8 byNum;
  __list List[10];
  int bUseNpcLink;
};
#pragma pack(pop)

// unmannedtrader_re_regist_request_clzo.h
#pragma pack(push, 1)
struct _unmannedtrader_re_regist_request_clzo
{
  struct __list
  {
    bool bRegist;
    unsigned __int16 wItemSerial;
    unsigned __int8 byAmount;
    unsigned __int8 byItemTableCode;
    unsigned __int16 wItemIndex;
    unsigned int dwPrice;
    unsigned int dwRegistSerial;
  };
  unsigned __int8 byTaxRate;
  unsigned __int8 byRegedNum;
  __list List[10];
};
#pragma pack(pop)

// unmannedtrader_search_list_request_clzo.h
#pragma pack(push, 1)
struct _unmannedtrader_search_list_request_clzo
{
  unsigned __int8 byStoreIndex;
  unsigned __int8 byDivision;
  unsigned __int8 byClass;
  unsigned __int8 bySubClass;
  unsigned __int8 bySortType;
  unsigned int dwVer;
  unsigned __int8 byPage;
  int bUseNpcLink;
};
#pragma pack(pop)
