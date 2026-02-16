#pragma once

#pragma pack(push, 1)
struct _limited_sale_item
{
  unsigned __int8 byTableCode;
  unsigned int dwIndex;
  unsigned __int16 wNum;
};

struct _limited_sale_info
{
  unsigned __int8 byCount;
  unsigned __int8 byDiscount;
  _limited_sale_item item[20];
};
#pragma pack(pop)

