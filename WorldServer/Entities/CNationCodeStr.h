#pragma once

#include "IdaCompat.h"

class  __declspec(align(2)) CNationCodeStr
{
public:
  CNationCodeStr() = default;
  CNationCodeStr(int code, const char *codeStr);
  bool IsNULL();
  int GetKey();
  char *GetStr();

  int m_iCode;
  char m_szStr[3];
};

bool operator==(const CNationCodeStr &lhs, const CNationCodeStr &rhs);
