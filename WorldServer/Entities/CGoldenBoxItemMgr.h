#pragma once

#include "IdaCompat.h"

struct __cppobj __declspec(align(1)) _golden_box_item_event
{
  unsigned __int8 m_event_status;
};

class __cppobj CGoldenBoxItemMgr
{
public:
  static CGoldenBoxItemMgr *Instance();
  bool Initialize();
  unsigned __int8 Get_Event_Status();

  struct _BoxItemOpen_output
  {
    unsigned int m_dwOutputIndex;
    unsigned int m_dwEffectCode;
  };

  virtual ~CGoldenBoxItemMgr() = default;

  _golden_box_item_event m_golden_box_event;
};

