#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"

class CPlayer;
struct _AVATOR_DATA;

class __cppobj CPcBangFavor
{
public:
  static CPcBangFavor *Instance();
  bool Initialzie();
  bool LoadPcBangData();
  bool IsEnable();
  int ClassCodePasing(_AVATOR_DATA *pData, CPlayer *pOne);
  bool PcBangGiveItem(CPlayer *pOne, int dwRecIndex, unsigned __int8 *bySeletItemIndex, int nSelectCount);
  void PcBangDeleteItem(CPlayer *pOne);

  unsigned int m_PcRoomIndex;
  int m_bEnable;
  CRecordData m_tblPcRoomData;
};
