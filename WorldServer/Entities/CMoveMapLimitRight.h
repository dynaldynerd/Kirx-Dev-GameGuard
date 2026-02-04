#pragma once

#include "IdaCompat.h"

class CPlayer;

class __cppobj __declspec(align(8)) CMoveMapLimitRight
{
public:
  CMoveMapLimitRight(int iType);
  virtual ~CMoveMapLimitRight();

  static CMoveMapLimitRight *Create(int iType);

  virtual void Load(CPlayer *pkPlayer);
  virtual void LogIn(CPlayer *pkPlayer);
  virtual void CreateComplete(CPlayer *pkPlayer);
  virtual void LogOut(CPlayer *pkPlayer);
  virtual bool IsHaveRight();
  virtual void SetFlag(int iType, bool bFlag);

  void CleanUp();
  int GetType() const;

  int m_iType;
};
