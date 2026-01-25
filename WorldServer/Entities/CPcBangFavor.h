#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

class __cppobj CPcBangFavor
{
public:
  static CPcBangFavor *Instance();
  bool Initialzie();

  unsigned int m_PcRoomIndex;
  int m_bEnable;
  CRecordData m_tblPcRoomData;
};
