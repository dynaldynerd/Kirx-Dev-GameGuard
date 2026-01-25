#pragma once

#include "IdaCompat.h"

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <functional>
#include <hash_map>
#include <cstring>

#include "CNationCodeStr.h"

template <typename TKey, typename TValue>
class __cppobj CHashMapPtrPool
{
public:
  bool m_bCleanUp;
  stdext::hash_map<TKey, TValue *, stdext::hash_compare<TKey, std::less<TKey>>> m_mapData;

  int regist(TValue *value)
  {
    if (value == nullptr)
    {
      return -1;
    }
    m_mapData[value->m_iCode] = value;
    return 0;
  }

  bool get(const TKey &key, TValue **outValue) const
  {
    if (outValue == nullptr)
    {
      return false;
    }
    auto it = m_mapData.find(key);
    if (it == m_mapData.end())
    {
      *outValue = nullptr;
      return false;
    }
    *outValue = it->second;
    return true;
  }

  bool findkey(const TValue *value, TKey *outKey) const
  {
    if (value == nullptr || outKey == nullptr)
    {
      return false;
    }
    for (const auto &pair : m_mapData)
    {
      if (std::strncmp(pair.second->m_szStr, value->m_szStr, 2) == 0)
      {
        *outKey = pair.first;
        return true;
      }
    }
    return false;
  }
};

class __cppobj CNationCodeStrTable
{
public:
  CNationCodeStrTable() = default;
  ~CNationCodeStrTable() = default;

  bool Init();
  int RegistCode();
  int GetCode(const char *code) const;
  const char *GetStr(int code) const;

  CHashMapPtrPool<int, CNationCodeStr> m_kTable;
};
