#pragma once

#include "IdaCompat.h"

struct _base_fld;
class CMapData;
class CItemStore;

struct __cppobj _object_create_setdata
{
  _base_fld *m_pRecordSet;
  CMapData *m_pMap;
  int m_nLayerIndex;
  float m_fStartPos[3];

  _object_create_setdata()
  {
    m_pRecordSet = nullptr;
    m_pMap = nullptr;
    m_nLayerIndex = 0;
    m_fStartPos[0] = m_fStartPos[1] = m_fStartPos[2] = 0.0f;
  }
};

struct __cppobj _character_create_setdata : _object_create_setdata
{
};

struct __cppobj __declspec(align(8)) _npc_create_setdata : _character_create_setdata
{
  CItemStore *m_pLinkItemStore;
  unsigned __int8 m_byRaceCode;

  _npc_create_setdata()
  {
    m_pLinkItemStore = nullptr;
    m_byRaceCode = 0;
  }
};
