#pragma once

#include "IdaCompat.h"

class __cppobj CRtc
{
public:
  CRtc();
  ~CRtc();

  static CRtc *GetIntance();
  void Reg_Fn();
  static int RTC_Fn(int errType, const wchar_t *file, int line, const wchar_t *module, const wchar_t *format, ...);

private:
  CRtc(const CRtc &) = delete;
  CRtc &operator=(const CRtc &) = delete;

  static CRtc *ms_Instance;
};
