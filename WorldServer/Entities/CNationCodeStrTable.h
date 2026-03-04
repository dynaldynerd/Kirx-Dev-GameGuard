#pragma once

#include "IdaCompat.h"

#include "CHashMapPtrPool.h"
#include "CNationCodeStr.h"

class  CNationCodeStrTable
{
public:
  CNationCodeStrTable();
  ~CNationCodeStrTable();

  bool Init();
  int RegistCode();
  int GetCode(const char *code);
  const char *GetStr(int code);

  CHashMapPtrPool<int, CNationCodeStr> m_kTable;
};
