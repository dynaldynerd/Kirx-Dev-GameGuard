#pragma once

#include "IdaCompat.h"

class __cppobj cStaticMember_Player
{
  int _nMaxLv;
  long double *_pLimExp;
  static cStaticMember_Player *_pInstance;

  cStaticMember_Player();
  ~cStaticMember_Player();
  bool loadLimitExpData();

public:
  static cStaticMember_Player *Instance();
  static void Release();
  bool Initialize();
  int GetMaxLv();
  long double GetLimitExp(int lv);
};
