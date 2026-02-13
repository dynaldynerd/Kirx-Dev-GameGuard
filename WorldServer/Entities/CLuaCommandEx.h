#pragma once

#include "IdaCompat.h"

/* 1677 */
class __cppobj __declspec(align(8)) CLuaCommandEx : public CLuaCommand
{
public:
  ~CLuaCommandEx() override;

  class _State
  {
  public:
    bool m_bExist;

    _State();
    bool operator==(const _State &rhs) const;

    static _State ms_cEmpty;
  };

  char *GetScriptName();
  void SetCmd(unsigned __int8 byType, const char *strScriptName, const char *strName);

  char m_strScriptName[260];
};

