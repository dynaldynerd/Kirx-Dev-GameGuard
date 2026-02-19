#pragma once

#include "IdaCompat.h"

struct _worlddb_golden_box_item
{
  struct _golden_box_item_info
  {
    int nGoldencode;
    unsigned __int16 wGoldencount;
  };

  int nSerial;
  unsigned __int8 bydck;
  unsigned int dwStarterBoxCnt;
  int nBox_item_code[2];
  unsigned __int16 wBox_item_max[2];
  unsigned __int8 bygolden_item_num[2];
  _golden_box_item_info List[2][100];
};

