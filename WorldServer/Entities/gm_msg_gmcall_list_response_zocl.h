#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _gm_msg_gmcall_list_response_zocl
{
  struct _call_node
  {
    unsigned int dwSerial;
    char m_wszUserName[17];
  };

  int m_nCurrPageUserNum;
  int m_nCurrPageIndex;
  int m_MaxPage;
  int m_nMaxUser;
  _call_node m_arCallNode[10];

  static void Init(_gm_msg_gmcall_list_response_zocl *msg)
  {
    memset_0(msg, 0, sizeof(_gm_msg_gmcall_list_response_zocl));
  }

  unsigned __int16 size() const
  {
    return static_cast<unsigned __int16>(sizeof(_gm_msg_gmcall_list_response_zocl));
  }
};
#pragma pack(pop)

static_assert(sizeof(_gm_msg_gmcall_list_response_zocl) == 0xE2, "gm call list packet size mismatch");
