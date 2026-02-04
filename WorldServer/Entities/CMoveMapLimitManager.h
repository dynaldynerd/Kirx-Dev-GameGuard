#pragma once

#include "IdaCompat.h"

#include "CMoveMapLimitInfoList.h"
#include "CMoveMapLimitRightInfoList.h"

class __cppobj CMoveMapLimitManager
{
public:
  static CMoveMapLimitManager *Instance();
  bool Init();
  bool Request(
    int iLimitType,
    int iRequetType,
    int iMapInx,
    unsigned int dwStoreRecordIndex,
    int iUserInx,
    char *pRequest);
  bool RequestElanMapUserForceMoveHQ();

  CMoveMapLimitRightInfoList m_kRightInfo;
  CMoveMapLimitInfoList m_kLimitInfo;
};
