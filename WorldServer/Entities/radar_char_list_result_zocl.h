#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _radar_char_list_result_zocl
{
  struct __unaligned __declspec(align(1)) _char_info
  {
    unsigned __int8 m_byCharType;
    float m_fPos[2];
  };

  bool bEnd;
  unsigned __int8 byListNum;
  _char_info CharInfo[50];

  _radar_char_list_result_zocl();
  __int64 size();
};
#pragma pack(pop)
