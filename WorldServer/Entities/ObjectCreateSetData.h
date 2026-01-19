#pragma once

#include "IdaCompat.h"

struct _base_fld;
class CMapData;

struct _object_create_setdata
{
  _base_fld *m_pRecordSet;
  CMapData *m_pMap;
  int m_nLayerIndex;
  float m_fStartPos[3];
};
