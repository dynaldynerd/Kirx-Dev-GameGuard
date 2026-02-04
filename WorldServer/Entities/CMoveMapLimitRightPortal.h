#pragma once

#include "IdaCompat.h"

#include "CMoveMapLimitRight.h"

struct MiningTicket;

class __cppobj __declspec(align(8)) CMoveMapLimitRightPortal : public CMoveMapLimitRight
{
public:
  CMoveMapLimitRightPortal(int iType);

  void Load(CPlayer *pkPlayer) override;
  void CreateComplete(CPlayer *pkPlayer) override;
  void LogOut(CPlayer *pkPlayer) override;
  bool IsHaveRight() override;
  void SetFlag(int iType, bool bFlag) override;

  MiningTicket *m_pkRight;
  bool m_bNotifyForceMoveStartPosition;
};
