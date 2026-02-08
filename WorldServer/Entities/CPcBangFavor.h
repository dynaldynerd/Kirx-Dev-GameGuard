#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

class CPlayer;

class __cppobj CPcBangFavor
{
public:
  static CPcBangFavor *Instance();
  bool Initialzie();
  bool LoadPcBangData();
  void PcBangDeleteItem(CPlayer *pOne);

  unsigned int m_PcRoomIndex;
  int m_bEnable;
  CRecordData m_tblPcRoomData;
};
