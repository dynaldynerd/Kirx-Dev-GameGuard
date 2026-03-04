#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"
#include "CUnmannedTraderSchedule.h"

class CPlayer;
class CMapData;
class CReturnGateCreateParam;
struct _open_return_gate_inform_zocl;

class  CReturnGate : public CGameObject
{
public:
  static unsigned int ms_dwSerialCnt;

  CReturnGate(_object_id *pID);
  ~CReturnGate();

  CUnmannedTraderSchedule::STATE m_eState;
  CPlayer *m_pkOwner;
  unsigned int m_dwOwnerSerial;
  CMapData *m_pDestMap;
  float m_fBindPos[3];
  unsigned int m_dwCloseTime;

  void Clear();
  unsigned __int16 GetIndex();
  CPlayer *GetOwner();
  bool IsOpen();
  bool IsClose();
  bool IsValidOwner();
  bool IsValidPosition(float *pfCurPos);
  void GetInfo(_open_return_gate_inform_zocl *Info);
  bool Open(CReturnGateCreateParam *pParam);
  int Enter(CPlayer *pkObj);
  void Close();
  void SendMsg_Create();
  void SendMsg_FixPosition(int n) override;
  void SendMsg_Destroy();
  void SendMsg_MovePortal(CPlayer *pkObj);
};
