#pragma once

#include "IdaCompat.h"

struct qry_case_select_golden_box_item
{
  struct __declspec(align(4)) _db_golden_box_item_List
  {
    int ncode;
    unsigned __int16 wcount;
  };

  unsigned __int8 bydck;
  unsigned int dwStarterBoxCnt;
  int nBoxcode[2];
  unsigned __int16 wBoxMax[2];
  unsigned __int8 bygolden_item_num[2];
  _db_golden_box_item_List List[2][100];
};
