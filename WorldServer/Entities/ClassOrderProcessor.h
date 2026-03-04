#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class  ClassOrderProcessor : public ElectProcessor
{
public:
  static ClassOrderProcessor *_pkInstance;
  ClassOrderProcessor();
  ~ClassOrderProcessor() override;
  bool Initialize() override;
  int Doit(Cmd cmd, CPlayer *pkPlayer, char *pszData) override;

  static ClassOrderProcessor *Instance();
  int _CheckUserInfo(unsigned __int8 byRace, unsigned __int8 byClassType, CPlayer *pUser);
  int _QueryAppoint(CPlayer *pOne, char *pData);
  int _RequestAppoint(CPlayer *pOne, char *pData);
  int _ResponseAppoint(CPlayer *pOne, char *pData);
  int _RequestDischarge(CPlayer *pOne, char *pData);
  void SendMsg_PatriarchInform(CPlayer *pOne);

  void SendMsg_QueryAppointResult(
    unsigned __int16 wIndex,
    unsigned __int8 byRet,
    unsigned __int8 byClassType,
    char *pwszAvatorName);
  void UpdatePacket(unsigned __int8 byRace, unsigned __int8 byClassType);

  unsigned __int8 _byPtType[2];
  _pt_appoint_inform_request_zocl _kSend[3];
};

