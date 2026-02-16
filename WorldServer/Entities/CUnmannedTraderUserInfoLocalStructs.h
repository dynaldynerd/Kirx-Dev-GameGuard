#pragma once

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _unmannedtrader_sell_wait_item_inform_zocl
{
  struct __unaligned __declspec(align(2)) __list
  {
    unsigned __int16 wItemSerial;
    unsigned int dwSellDalant;
    unsigned int dwTax;
  };

  unsigned __int8 byNum;
  unsigned int dwTotalSellDalant;
  unsigned int dwTotalTaxDalant;
  unsigned int dwCurInvenDalant;
  __list List[10];

  __int64 size() const
  {
    unsigned __int8 clamped = byNum;
    if (clamped > 10)
    {
      clamped = 0;
    }
    return 116 - 10LL * (10 - clamped);
  }
};
#pragma pack(pop)

