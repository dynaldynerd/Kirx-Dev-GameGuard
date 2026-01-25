#include "pch.h"

#include "CPcBangFavor.h"

CPcBangFavor *CPcBangFavor::Instance()
{
  static CPcBangFavor s_instance;
  return &s_instance;
}

bool CPcBangFavor::Initialzie()
{
  return true;
}
