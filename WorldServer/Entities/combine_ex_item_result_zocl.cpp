#include "pch.h"

#include "combine_ex_item_result_zocl.h"

_combine_ex_item_result_zocl::__item::__item()
{
  Init();
}

void _combine_ex_item_result_zocl::__item::Init()
{
  _COMBINEKEY::SetRelease(&Key);
  dwDur = 0;
  dwUpt = 0xFFFFFFF;
}

_combine_ex_item_result_zocl::_Result_ItemList_Buff::_Result_ItemList_Buff()
{
  Init();
}

void _combine_ex_item_result_zocl::_Result_ItemList_Buff::Init()
{
  byItemListNum = 0;
  for (int i = 0; i < 24; ++i)
  {
    RewardItemList[i].Init();
  }
}

_combine_ex_item_result_zocl::_combine_ex_item_result_zocl()
{
}
