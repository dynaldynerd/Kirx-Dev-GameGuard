#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"
#include "CSetItemType.h"

class  CSUItemSystem
{
public:
  CSUItemSystem();
  ~CSUItemSystem();
  static CSUItemSystem *SCSUITEMSYSTEM;

  static CSUItemSystem *Instance();
  bool SUItemSystem_Init();
  bool SUItemSystem_CheckData();
  char SUItemSystem_UnInit();
  CSUItemSystem *GetCRecordData_SetItem();
  CSetItemType *GetCSetItemType();
  int GetSetItemTableInfo(int dwSetItemEff, char *pStrCode, int nBufSize);

  void Class_Init();

  CRecordData m_SUOrigin[1];
  CSetItemType m_SetItemType;
  unsigned __int8 m_bySUItemCount;
  bool m_bLoadData[1];
  bool m_bChangeData[1];
};
