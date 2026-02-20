#include "pch.h"

#include "CMoveMapLimitEnviromentValues.h"

#include "CMapData.h"
#include "CMapOperation.h"

#include <cstring>

int CMoveMapLimitEnviromentValues::ELAN_MAP_CODE;
int CMoveMapLimitEnviromentValues::ELAN_1TH_LIMIT_NPC_RECORD_INDEX;

bool CMoveMapLimitEnviromentValues::Init()
{
  for (int j = 0; j < g_MapOper.m_nMapNum; ++j)
  {
    if (g_MapOper.m_Map != nullptr && g_MapOper.m_Map[j].m_pMapSet != nullptr
      && std::strcmp(g_MapOper.m_Map[j].m_pMapSet->m_strCode, "Elan") == 0)
    {
      ELAN_MAP_CODE = g_MapOper.m_Map[j].m_nMapCode;
      return true;
    }
  }
  return true;
}

