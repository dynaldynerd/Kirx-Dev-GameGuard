#pragma once

#include "IdaCompat.h"

class __cppobj __declspec(align(2)) CNationCodeStr
{
public:
  CNationCodeStr() = default;
  CNationCodeStr(int code, const char *codeStr);
  char *GetStr();

  int m_iCode;
  char m_szStr[3];
};
