#pragma once

#include "IdaCompat.h"

#include "CMoveMapLimitRight.h"

struct MiningTicket;

class __cppobj __declspec(align(8)) CMoveMapLimitRightPortal : public CMoveMapLimitRight
{
public:
  MiningTicket *m_pkRight;
  bool m_bNotifyForceMoveStartPosition;
};
