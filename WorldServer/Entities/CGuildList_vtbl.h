#pragma once

#include "IdaCompat.h"

class CGuildList;

class CGuildList_vtbl
{
public:
  void (__fastcall *destructor)(CGuildList *pThis);
};
