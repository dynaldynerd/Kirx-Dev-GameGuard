#pragma once

#include "IdaCompat.h"

#include <vector>

class CMoveMapLimitInfo;

class __cppobj CMoveMapLimitInfoList
{
public:
  std::vector<CMoveMapLimitInfo *> m_vecLimitInfo;
};
