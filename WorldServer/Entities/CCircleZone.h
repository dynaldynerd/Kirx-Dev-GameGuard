#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

class CMapData;
struct _dummy_position;

class CCircleZone : public CCharacter
{
public:
  enum GCZ_STATE : __int32
  {
    CZ_NONE = -1,
    CZ_INIT = 0,
    CZ_CREATE = 1,
    CZ_DESTROY = 2,
  };

  CCircleZone();
  ~CCircleZone();
  char Create(CMapData *pkMap, unsigned __int8 byColor);
  void Destroy();
  bool Init(unsigned int uiMapInx, int iPlayerInx, int iNth, unsigned __int16 wInx, CMapData *pkMap);
  unsigned __int8 GetColor() const;
  unsigned __int8 Goal(CMapData *pkMap, const float *pfCurPos);
  bool IsNearPosition(float *pfCurPos);
  int GetPortalInx() const;
  void SendMsgCreate();
  void SendMsgGoal();
  void SendMsg_FixPosition(int n) override;

  static unsigned int ms_dwSerialCnt;

  GCZ_STATE m_eState;
  int m_iPortalInx;
  unsigned __int8 m_byColor;
  _dummy_position *m_pkGoalPos;
};

