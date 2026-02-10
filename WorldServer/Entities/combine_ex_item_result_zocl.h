#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

#pragma pack(push, 1)
struct __cppobj _combine_ex_item_result_zocl
{
  struct __item
  {
    _COMBINEKEY Key;
    unsigned int dwDur;
    unsigned int dwUpt;

    __item();
    void Init();
  };

  struct __unaligned __declspec(align(1)) _Result_ItemList_Buff
  {
    unsigned __int8 byItemListNum;
    __item RewardItemList[24];

    _Result_ItemList_Buff();
    void Init();
  };

  unsigned __int8 byErrCode;
  unsigned __int8 byDlgType;
  __unaligned __declspec(align(1)) unsigned int dwDalant;
  __unaligned __declspec(align(1)) unsigned int dwCheckKey;
  unsigned __int8 bySelectItemCount;
  _Result_ItemList_Buff ItemBuff;
  unsigned int dwResultEffectType;
  unsigned int dwResultEffectMsgCode;

  _combine_ex_item_result_zocl();
};
#pragma pack(pop)
