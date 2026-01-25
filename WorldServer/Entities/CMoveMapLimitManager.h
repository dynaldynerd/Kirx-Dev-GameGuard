#pragma once

#include "IdaCompat.h"

#include "CMoveMapLimitInfoList.h"
#include "CMoveMapLimitRightInfoList.h"

class __cppobj CMoveMapLimitManager
{
public:
  static CMoveMapLimitManager *Instance();
  bool Init();

  CMoveMapLimitRightInfoList m_kRightInfo;
  CMoveMapLimitInfoList m_kLimitInfo;
};
