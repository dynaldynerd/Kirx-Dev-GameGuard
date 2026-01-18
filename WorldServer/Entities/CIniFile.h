#pragma once

#include "IdaCompat.h"

/* 1438 */
struct __cppobj CIniFile
{
  char m_strPath[260];
  std::vector<INI_Section *> m_SectionList;
  virtual ~CIniFile() = default;
};
