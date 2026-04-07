#pragma once

#include "IdaCompat.h"

#include <cstring>

struct _ITEM_INTEGRITY_INFO
{
  static constexpr int kMaxEntries = 300;

  int m_nCount;
  int m_nBeforeIndex[kMaxEntries];
  int m_nChangedIndex[kMaxEntries];

  _ITEM_INTEGRITY_INFO()
  {
    Init();
  }

  void Init()
  {
    std::memset(this, 0, sizeof(*this));
  }

  unsigned __int16 FindChangedIndex(unsigned __int16 itemIndex) const
  {
    for (int index = 0; index < m_nCount; ++index)
    {
      if (m_nBeforeIndex[index] == itemIndex)
      {
        return static_cast<unsigned __int16>(m_nChangedIndex[index]);
      }
    }

    return itemIndex;
  }
};
