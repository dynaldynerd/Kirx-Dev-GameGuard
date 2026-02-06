#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class __cppobj __declspec(align(4)) ClassOrderProcessor : public ElectProcessor
{
public:
  ClassOrderProcessor();

  unsigned __int8 _byPtType[2];
  _pt_appoint_inform_request_zocl _kSend[3];
};
