#pragma once

#include "IdaCompat.h"
#include "USArray.h"

#include "CLuaEventNode.h"

class  CLuaEventMgr
{
public:
  static CLuaEventMgr *Instance();
  static void Destroy();

  bool InitSDM();
  void Loop();
  CLuaEventNode *NewEvent();
  void RemoveEvent(CLuaEventNode *pEvent);
  bool AttachEvent(CLuaEventNode *pEvent);
  bool DettachEvent(CLuaEventNode *pEvent);
  CLuaEventNode *SearchEvent(const char *strFileName);

private:
  CLuaEventMgr();
  ~CLuaEventMgr();

  static CLuaEventMgr *ms_Instance;

  US::CArrayEx<CLuaEventNode, CLuaEventNode::_State> m_LuaEventArEx;
};
