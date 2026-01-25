#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _consume_item_list
{
  char m_itmNeedItemCode[8];
  int m_nNeedItemCount;
};

struct _cont_param_list
{
  int m_nContParamCode;
  int m_nContParamIndex;
  float m_fContValue[7];
};
#pragma pack(pop)
