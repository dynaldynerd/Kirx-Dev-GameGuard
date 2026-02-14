#pragma once

#include "IdaCompat.h"
#include "AsyncLogTypes.h"
#include "CFrameRate.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

#include <cstdarg>
#include <functional>
#ifndef _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#endif
#include <hash_map>
#include <vector>

class CAsyncLogInfo;
class CAsyncLogBufferList;

class __cppobj CAsyncLogger
{
public:
  CAsyncLogger();
  ~CAsyncLogger();

  static constexpr ASYNC_LOG_TYPE ALT_ASYNC_LOGGER_SYSTEM_LOG = ::ALT_ASYNC_LOGGER_SYSTEM_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_PERFORMANCE_COUNTER_SYSTEM = ::ALT_PERFORMANCE_COUNTER_SYSTEM;
  static constexpr ASYNC_LOG_TYPE ALT_PERFORMANCE_MAIN_LOOP = ::ALT_PERFORMANCE_MAIN_LOOP;
  static constexpr ASYNC_LOG_TYPE ALT_PERFORMANCE_MAIN_DQS = ::ALT_PERFORMANCE_MAIN_DQS;
  static constexpr ASYNC_LOG_TYPE ALT_PERFORMANCE_LOG_DQS = ::ALT_PERFORMANCE_LOG_DQS;
  static constexpr ASYNC_LOG_TYPE ALT_PERFORMANCE_CASH_DQS = ::ALT_PERFORMANCE_CASH_DQS;
  static constexpr ASYNC_LOG_TYPE ALT_PERFORMANCE_NETWORK_DQS = ::ALT_PERFORMANCE_NETWORK_DQS;
  static constexpr ASYNC_LOG_TYPE ALT_FIREGUARD_DETECT_LOG = ::ALT_FIREGUARD_DETECT_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_HONOR_GUILD_LOG = ::ALT_HONOR_GUILD_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_CLASSREFINE_SERVICE_LOG = ::ALT_CLASSREFINE_SERVICE_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_BUY_CASH_ITEM_LOG = ::ALT_BUY_CASH_ITEM_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_HACKSHIELD_SYSTEM_LOG = ::ALT_HACKSHIELD_SYSTEM_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_APEX_SYSTEM_LOG = ::ALT_APEX_SYSTEM_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_GETGOLDBAR_LOG = ::ALT_GETGOLDBAR_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_GETEVENTCOUPON_LOG = ::ALT_GETEVENTCOUPON_LOG;
  static constexpr ASYNC_LOG_TYPE ALT_MAX = ::ALT_MAX;
  static constexpr ASYNC_LOG_TYPE ALT_NONE = ::ALT_NONE;

  static CAsyncLogger *Instance();
  bool Regist(
    ASYNC_LOG_TYPE eType,
    const char *directory,
    const char *filePrefix,
    bool isDaily,
    unsigned int dwUpdateFileNameDelay);
  bool Log(int iType, const char *szLog);
  bool LogFromArg(int iType, char *fmt, va_list arg_ptr);
  bool FormatLog(int iType, const char *fmt, ...);
  void Log(const char *szFileName, const char *szLog, int iLenStr);
  void SystemLog(const char *fmt, ...);
  int Init();
  void Loop();
  void ProcWrite();
  static void __cdecl ProcThread(void *param);

  static bool m_bProcThread;

private:
  CFrameRate m_FrameRate;
  CLogFile m_logLoading;
  CMyTimer m_kCheckUpdateLogFileNameDelay;
  stdext::hash_map<int, CAsyncLogInfo *, stdext::hash_compare<int, std::less<int>>> m_mapLogInfo;
  CAsyncLogInfo *m_pSystemLogInfo;
  CNetIndexList m_klistEmpty;
  CNetIndexList m_klistProc;
  std::vector<unsigned long> m_vecPushList;
  CAsyncLogBufferList *m_kBufferList;
};

