#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class __cppobj __declspec(align(4)) ClassOrderProcessor : public ElectProcessor
{
public:
  ClassOrderProcessor();
  static ClassOrderProcessor *Instance();
  void SendMsg_QueryAppointResult(
    unsigned __int16 wIndex,
    unsigned __int8 byRet,
    unsigned __int8 byClassType,
    char *pwszAvatorName);
  void UpdatePacket(unsigned __int8 byRace, unsigned __int8 byClassType);

  unsigned __int8 _byPtType[2];
  _pt_appoint_inform_request_zocl _kSend[3];
};
