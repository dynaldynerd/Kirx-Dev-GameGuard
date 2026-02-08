#pragma once

#include "IdaCompat.h"

#include "CHashMapPtrPool.h"
#include "CNationCodeStr.h"

class __cppobj CNationCodeStrTable
{
public:
  CNationCodeStrTable() = default;
  ~CNationCodeStrTable() = default;

  bool Init();
  int RegistCode();
  int GetCode(const char *code);
  const char *GetStr(int code);

  CHashMapPtrPool<int, CNationCodeStr> m_kTable;
};
