#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "ElectProcessor.h"

struct _DB_QRY_SYN_DATA;

class  PatriarchElectProcessor
{
public:
  PatriarchElectProcessor();
  static PatriarchElectProcessor *_pkInstance;
  static PatriarchElectProcessor *Instance();
  void Destroy();
  bool Initialize();
  char LoadDatabae();
  void Loop();
  bool InitProcess();
  void TimeCheck(unsigned __int16 wDayOfWeek, unsigned __int16 wHour);
  unsigned int GetElectSerial();
  ElectProcessor::ProcessorType GetProcessorType();
  bool GetTimeCheck();
  void SetTimeCheck(bool bFlag);
  bool ForceChangeProcessor(ElectProcessor::ProcessorType eProc);
  unsigned int GetCurrPatriarchElectSerial();
  void SetCurrPatriarchElectSerial(unsigned int dwSerial);
  char Doit(Cmd eCmd, CPlayer *pOne, char *pdata);
  void PushDQSCheckInvalidChar();
  void SendMsg_ResultCode(unsigned int n, unsigned __int8 byCode);
  void SendMsg_ConnectNewUser(CPlayer *pOne);
  __int64 Insert_Elect();
  __int64 Update_Elect();
  char LoadElectState();
  __int64 Request_Refund(char *pData);
  __int64 Insert_PatrirchItemChargeRefund(char *pData);
  bool CheatClearPatriarch();
  bool CheatSetPatriarch(CPlayer *pOne, int eClass);
  void CompleteInsertElect();
  void CompleteSelectElect();
  void CompleteRequestRefund(_DB_QRY_SYN_DATA *pData);
  void CompleteItemChargeRefund(_DB_QRY_SYN_DATA *pData);
  void CompleteInsertPatriarch(_DB_QRY_SYN_DATA *pData);
  void CompleteCheckInvalidChar(unsigned __int8 byProc);

  unsigned int m_dwNonvoteCnt[3];
  unsigned int m_dwTotalVoteCnt[3];
  unsigned int m_dwHighGradeNum[3];
  ElectProcessor::ProcessorType _eProcessType;
  ElectProcessor *_kRunningProcessor;
  ElectProcessor *_kProcessor[6];
  bool _bTimeCheck;
  bool _bInitProce;
  unsigned int _dwNextCheckTime;
  unsigned int _dwNextCheckDay;
  unsigned int _dwNextScoreUpdateTime;
  unsigned int _dwElectSerial;
  unsigned int _dwCurrPatriarchElectSerial;
  CLogFile _kSysLog;
};
