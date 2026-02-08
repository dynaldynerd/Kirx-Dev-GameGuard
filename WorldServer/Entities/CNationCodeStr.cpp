#include "pch.h"

#include "CNationCodeStr.h"

#include <cstring>

CNationCodeStr::CNationCodeStr(int code, const char *codeStr)
{
  m_iCode = code;
  if (codeStr != nullptr)
  {
    m_szStr[0] = codeStr[0];
    m_szStr[1] = codeStr[1];
    m_szStr[2] = '\0';
  }
  else
  {
    m_szStr[0] = '\0';
    m_szStr[1] = '\0';
    m_szStr[2] = '\0';
  }
}

char *CNationCodeStr::GetStr()
{
  return m_szStr;
}

bool CNationCodeStr::IsNULL()
{
  // this is not a stub
  return false;
}

int CNationCodeStr::GetKey()
{
  return m_iCode;
}

bool operator==(const CNationCodeStr &lhs, const CNationCodeStr &rhs)
{
  return _stricmp(lhs.m_szStr, rhs.m_szStr) == 0;
}
