#pragma once

#include "IdaCompat.h"

class  CNationCodeStr
{
public:
  CNationCodeStr() = default;
  ~CNationCodeStr();
  CNationCodeStr(int code, const char *codeStr);
  bool IsNULL();
  int GetKey();
  char *GetStr();

  int m_iCode;
  char m_szStr[3];
};

bool operator==(const CNationCodeStr &lhs, const CNationCodeStr &rhs);

