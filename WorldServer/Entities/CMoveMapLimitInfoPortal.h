#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CMoveMapLimitInfo.h"

struct _dummy_position;
class CMyTimer;

class __cppobj __declspec(align(8)) CMoveMapLimitInfoPortal : public CMoveMapLimitInfo
{
public:
  enum FORCE_MOVE_HQ_STATE : int
  {
    FMHS_WAIT = 0,
    FMHS_WAIT_NOTIFY_FORCE_MOVE = 1,
    FMHS_NOTIFY_FORCE_MOVE = 2,
    FMHS_WAIT_FORCE_MOVE = 3,
    FMHS_FORCE_MOVE = 4,
  };

  _dummy_position *m_pkSrcDummy;
  _dummy_position *m_pkDestDummy;
  _dummy_position *m_pkRegenDummy;
  std::vector<char *> m_vecAllowDummyCode;
  FORCE_MOVE_HQ_STATE m_eNotifyForceMoveHQState;
  CMyTimer *m_pkNotifyForceMoveHQTimer;
  unsigned int m_uiProcNotifyInx;
};
