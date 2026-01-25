#pragma once

#include "IdaCompat.h"

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <functional>
#include <hash_map>

#include "CNationCodeStr.h"

template <typename TKey, typename TValue>
class __cppobj CHashMapPtrPool
{
public:
  bool m_bCleanUp;
  stdext::hash_map<TKey, TValue *, stdext::hash_compare<TKey, std::less<TKey>>> m_mapData;
};

class __cppobj CNationCodeStrTable
{
public:
  CNationCodeStrTable() = default;
  ~CNationCodeStrTable() = default;

  bool Init();
  int GetCode(const char *code) const;
  const char *GetStr(int code) const;

  CHashMapPtrPool<int, CNationCodeStr> m_kTable;
};
