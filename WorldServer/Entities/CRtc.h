#pragma once

#include "IdaCompat.h"

extern "C" {
typedef int(__cdecl *RTC_error_fnW)(int, const wchar_t *, int, const wchar_t *, const wchar_t *, ...);
RTC_error_fnW __cdecl RTC_SetErrorFuncW(RTC_error_fnW func);
}

extern LONG g_rtcRunning;
extern RTC_error_fnW g_rtcErrorFunc;

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
