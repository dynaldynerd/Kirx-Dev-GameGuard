#include "pch.h"

#include "GMCallMgr.h"

#include <cstdio>

#include "CPlayer.h"
#include "GlobalObjects.h"
#include "gm_msg_gmcall_list_response_zocl.h"

char GMCallMgr::SendResponseGMCall(CPlayer *pOne, int bCallState)
{
  if (!pOne)
  {
    return 0;
  }

  char msg[4]{};
  *reinterpret_cast<int *>(msg) = bCallState;

  unsigned __int8 type[2]{};
  type[0] = 55;
  type[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(pOne->m_ObjID.m_wIndex, type, msg, 4u);
  return 1;
}

char GMCallMgr::SendResponseGMList(CPlayer *pOne, int nCurrPageIndex)
{
  if (!pOne)
  {
    return 0;
  }

  unsigned int list[MAX_PLAYER + 1]{};
  int listCount = m_listGMRequestDataTask.CopyIndexList(list, MAX_PLAYER);
  int maxPage = (listCount - 1) / 10 + 1;
  if (nCurrPageIndex <= 0)
  {
    nCurrPageIndex = 0;
  }
  if (nCurrPageIndex >= maxPage)
  {
    nCurrPageIndex = maxPage - 1;
  }

  const int beginIndex = 10 * nCurrPageIndex;
  const int endIndex = beginIndex + 9;
  int outCount = 0;

  _gm_msg_gmcall_list_response_zocl response{};
  _gm_msg_gmcall_list_response_zocl::Init(&response);
  if (listCount > 0)
  {
    int current = beginIndex;
    while (current <= endIndex && current < listCount)
    {
      GMRequestData *requestData = &m_buffGMRequestData[list[current]];
      _gm_msg_gmcall_list_response_zocl::_call_node *node = &response.m_arCallNode[outCount];
      node->dwSerial = requestData->m_dwPlayerSerial;
      sprintf_s(node->m_wszUserName, "%s", requestData->m_wszUserName);
      ++current;
      ++outCount;
    }
  }

  response.m_nCurrPageUserNum = (listCount > 0) ? outCount : 0;
  response.m_nCurrPageIndex = (listCount > 0) ? nCurrPageIndex : 0;
  response.m_MaxPage = (listCount > 0) ? maxPage : 0;
  response.m_nMaxUser = listCount;

  unsigned __int8 type[2]{};
  type[0] = 55;
  type[1] = 5;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pOne->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&response),
    0xE2u);
  return 1;
}

void GMCallMgr::SendResponseAcceptResult(CPlayer *pOneGM, CPlayer *pOneUser, int nErrorCode)
{
  (void)pOneUser;

  char msg[4]{};
  *reinterpret_cast<int *>(msg) = nErrorCode;

  unsigned __int8 type[2]{};
  type[0] = 55;
  type[1] = 6;
  g_Network.m_pProcess[0]->LoadSendMsg(pOneGM->m_ObjID.m_wIndex, type, msg, 4u);
}

