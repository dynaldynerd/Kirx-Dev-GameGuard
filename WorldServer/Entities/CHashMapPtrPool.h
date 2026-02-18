#pragma once

#include "IdaCompat.h"

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <functional>
#include <hash_map>
#include <utility>

template <typename TKey, typename TValue>
class  CHashMapPtrPool
{
public:
  CHashMapPtrPool()
    : m_bCleanUp(true),
      m_mapData()
  {
  }

  ~CHashMapPtrPool()
  {
    if (m_bCleanUp)
    {
      cleanup();
    }
  }

  int regist(TValue *value)
  {
    if (value == nullptr || value->IsNULL())
    {
      return -1;
    }

    const TKey key = static_cast<TKey>(value->GetKey());
    if (m_mapData.find(key) != m_mapData.end())
    {
      return -2;
    }

    m_mapData.insert(std::make_pair(key, value));
    return 0;
  }

  bool get(const TKey &key, TValue **outValue)
  {
    auto it = m_mapData.find(key);
    if (it == m_mapData.end())
    {
      return false;
    }
    *outValue = it->second;
    return true;
  }

  bool findkey(const TValue *value, TKey *outKey)
  {
    for (const auto &pair : m_mapData)
    {
      if (pair.second && value && (*pair.second == *value))
      {
        *outKey = pair.first;
        return true;
      }
    }
    return false;
  }

  void cleanup()
  {
    for (const auto &pair : m_mapData)
    {
      operator delete(pair.second);
    }
  }

  bool m_bCleanUp;
  stdext::hash_map<TKey, TValue *, stdext::hash_compare<TKey, std::less<TKey>>> m_mapData;
};
