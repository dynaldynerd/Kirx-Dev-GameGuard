#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "CUnmannedTraderSchedule.h"
#include <vector>

struct _unmannedtrader_reserved_schedule_info;

class __cppobj CUnmannedTraderScheduler
{
public:
  static CUnmannedTraderScheduler *Instance();
  static void Destroy();

  CUnmannedTraderScheduler();
  ~CUnmannedTraderScheduler();
  bool Init();
  bool Load();
  void Loop();
  void SetLogger(CLogFile *pkLogger);
  void DoDayChangedWork();
  bool FindWaitItem();
  std::vector<CUnmannedTraderSchedule>::iterator FindItem(
    unsigned __int8 byType,
    unsigned int dwRegistSerial);
  void Update(_unmannedtrader_reserved_schedule_info *pkInfo);
  void ClearAll();
  void PushLoad();
  void CompleteClear(
    unsigned __int8 byDBQueryRet,
    unsigned __int8 byProcRet,
    unsigned __int8 byType,
    unsigned int dwRegistSerial);
  void CheatPushLoad();
  void Log(const char *fmt, ...);

  static CUnmannedTraderScheduler *ms_Instance;

  int m_iOldDay;
  bool m_bLoad;
  CMyTimer *m_pkTimer;
  CLogFile *m_pkLogger;
  std::vector<CUnmannedTraderSchedule>::iterator m_iterSchedule;
  std::vector<CUnmannedTraderSchedule> m_veckSchdule;
};
