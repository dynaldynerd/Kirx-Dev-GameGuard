#pragma once

#include "IdaCompat.h"

#include <vector>

class CMoveMapLimitInfo;

class __cppobj CMoveMapLimitInfoList
{
public:
  bool Init(std::vector<int> *vecRightTypeList);

  std::vector<CMoveMapLimitInfo *> m_vecLimitInfo;
};
