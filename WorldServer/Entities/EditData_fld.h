#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _edit_data_node
{
  char m_strEditItemCode[64];
  int m_nEditItemcount;
};

struct __cppobj _EditData_fld : _base_fld
{
  _edit_data_node m_Node[30];
};
#pragma pack(pop)
