#pragma once

#include "IdaCompat.h"

class CMerchant;

class __cppobj CMoveMapLimitInfo
{
public:
  enum OBJ_TYPE : int
  {
    OT_NONE = -1,
    OT_PORTAL = 0,
    MAX_TYPE_NUM = 1,
  };

  virtual ~CMoveMapLimitInfo() = default;

  OBJ_TYPE m_eType;
  unsigned int m_uiInx;
  int m_iMapInx;
  CMerchant *m_pStoreNPC;
};
