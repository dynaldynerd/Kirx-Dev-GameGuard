#pragma once

#include "IdaCompat.h"

#pragma pack(push, 2)
#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(2)) _combine_ex_item_accept_request_clzo
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
