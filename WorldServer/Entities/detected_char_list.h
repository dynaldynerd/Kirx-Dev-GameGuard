#pragma once

#include "IdaCompat.h"

struct _detected_char_list
{
#pragma pack(push, 1)
  struct _char_info
  {
    unsigned __int8 m_byCharType;
    float m_fPos[2];
  };
#pragma pack(pop)

  int nSize;
  _char_info CharInfoList[50];
};
