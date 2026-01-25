#include "pch.h"

#include "CNationCodeStrTable.h"

bool CNationCodeStrTable::Init()
{
  return true;
}

int CNationCodeStrTable::GetCode(const char *code) const
{
  if (code == nullptr || code[0] == '\0')
  {
    return -1;
  }
  return 0;
}

const char *CNationCodeStrTable::GetStr(int code) const
{
  (void)code;
  return "KR";
}
