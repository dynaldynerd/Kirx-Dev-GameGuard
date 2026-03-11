#include "pch.h"

#include "CEventLootTable.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CLogFile.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CEventLootTable::_event_drop::_event_drop()
  : wMagnifications(0),
    wRange(0),
    wDropCntOnce(0),
    wDropDelay(0),
    pNext(nullptr)
{
}

CEventLootTable::CEventLootTable()
  : m_pEventDropList(nullptr)
{
}

CEventLootTable::~CEventLootTable()
{
  _event_drop *next = nullptr;
  for (_event_drop *node = m_pEventDropList; node; node = next)
  {
    next = node->pNext;
    operator delete(node);
  }
  m_pEventDropList = nullptr;
}

void CEventLootTable::AddRecord(_event_drop *pEventDrop)
{
  if (m_pEventDropList)
  {
    for (_event_drop *node = m_pEventDropList; node; node = node->pNext)
    {
      if (!node->pNext)
      {
        node->pNext = pEventDrop;
        return;
      }
    }
  }
  else
  {
    m_pEventDropList = pEventDrop;
  }
}

bool CEventLootTable::ReadRecord()
{
  FILE *stream = nullptr;
  if (fopen_s(&stream, ".\\Initialize\\EventLooting.ini", "r+t"))
  {
    return false;
  }

  char buffer[516]{};
  char token0[64]{};
  char token1[64]{};
  char token2[64]{};
  char token3[64]{};
  char token4[64]{};
  char *tokens[5] = {token0, token1, token2, token3, token4};

  while (fgets(buffer, 512, stream))
  {
    if (buffer[0] == ';')
    {
      continue;
    }
    std::memset(token0, 0, sizeof(token0));
    std::memset(token1, 0, sizeof(token1));
    std::memset(token2, 0, sizeof(token2));
    std::memset(token3, 0, sizeof(token3));
    std::memset(token4, 0, sizeof(token4));

    if (ParsingCommandA(buffer, 5, tokens, 64) == 5)
    {
      auto *record = static_cast<_event_drop *>(operator new(80u));
      if (!record)
      {
        continue;
      }
      new (record) _event_drop();
      std::strcpy(record->strCode, token0);
      record->wMagnifications = static_cast<unsigned __int16>(std::atoi(token1));
      record->wRange = static_cast<unsigned __int16>(std::atoi(token2));
      record->wDropCntOnce = static_cast<unsigned __int16>(std::atoi(token3));
      record->wDropDelay = static_cast<unsigned __int16>(std::atoi(token4));
      record->pNext = nullptr;
      AddRecord(record);
    }
  }

  fclose(stream);
  return true;
}

CEventLootTable::_event_drop *CEventLootTable::GetRecord(const char *szRecordCode)
{
  for (_event_drop *node = m_pEventDropList; node; node = node->pNext)
  {
    if (!std::strcmp(node->strCode, szRecordCode))
    {
      return node;
    }
  }
  return nullptr;
}

