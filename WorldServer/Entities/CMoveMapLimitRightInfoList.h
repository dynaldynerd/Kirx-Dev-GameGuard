#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CMoveMapLimitRightInfo.h"

class __cppobj CMoveMapLimitRightInfoList
{
public:
  bool Init(const std::vector<int> *vecRightTypeList);

  std::vector<CMoveMapLimitRightInfo> m_vecRight;
};
