#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

class CMapData;
struct _dummy_position;

class CGravityStoneRegener : public CCharacter
{
public:
  enum GSR_STATE : __int32
  {
    GSR_NONE = -1,
    GSR_INIT = 0,
    GSR_CREATE = 1,
    GSR_REGEN = 2,
    GSR_TAKE = 3,
    GSR_DESTROY = 4,
    GSR_MAX = 6,
  };

  CGravityStoneRegener();
  bool Init(unsigned int uiMapInx, unsigned __int16 wInx, CMapData *pkMap);
  int GetPortalInx() const;
  void SendMsg_FixPosition(int n) override;

  GSR_STATE m_eState;
  int m_iPortalInx;
  _dummy_position *m_pkRegenPos;
};

