#include "pch.h"

#include "GMCallMgr.h"

#include <crtdbg.h>
#include <cstdio>
#include <mmsystem.h>

#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "gm_msg_gmcall_list_response_zocl.h"
#include "Packet/ZoneClientPacket.h"

GMRequestData::GMRequestData() : m_dwPlayerSerial(0), m_dwCallTimer(0), m_wszUserName{}
{
}

void GMRequestData::Set(unsigned int dwSerial, const char *pwszName, unsigned int dwTime)
{
  m_dwPlayerSerial = dwSerial;
  m_dwCallTimer = dwTime;
  std::memset(m_wszUserName, 0, sizeof(m_wszUserName));
  if (pwszName)
  {
    std::memcpy(m_wszUserName, pwszName, sizeof(m_wszUserName));
    m_wszUserName[16] = 0;
  }
}

GMCallMgr::GMCallMgr()
{
  Init();
}

void GMCallMgr::Init()
{
  InitReqBuff();
}

void GMCallMgr::InitReqBuff()
{
  m_listGMRequestDataEmpty.SetList(MAX_PLAYER);
  m_listGMRequestDataTask.SetList(MAX_PLAYER);

  for (unsigned int index = 0; index < MAX_PLAYER; ++index)
  {
    m_buffGMRequestData[index] = GMRequestData();
    m_listGMRequestDataEmpty.PushNode_Back(index);
  }
}

GMRequestData *GMCallMgr::PopReqEmptNode()
{
  unsigned int outIndex = 0;
  if (!m_listGMRequestDataEmpty.PopNode_Front(&outIndex))
  {
    _ASSERTE(false);
  }

  if (!m_listGMRequestDataTask.PushNode_Back(outIndex))
  {
    _ASSERTE(false);
  }

  if (outIndex >= MAX_PLAYER)
  {
    return nullptr;
  }

  return &m_buffGMRequestData[outIndex];
}

void GMCallMgr::PushReqNode(GMRequestData *pInst)
{
  unsigned int index = static_cast<unsigned int>(pInst - m_buffGMRequestData);
  if (index == static_cast<unsigned int>(-1) || index >= MAX_PLAYER)
  {
    _ASSERTE(false);
    return;
  }

  m_listGMRequestDataTask.FindNode(index);
  m_listGMRequestDataEmpty.PushNode_Back(index);
}

GMRequestData *GMCallMgr::GetGMRequestDataPtr(CPlayer *pOne)
{
  unsigned int list[MAX_PLAYER + 1]{};
  const int listCount = m_listGMRequestDataTask.CopyIndexList(list, MAX_PLAYER);
  for (int i = 0; i < listCount; ++i)
  {
    GMRequestData *requestData = &m_buffGMRequestData[list[i]];
    if (requestData->m_dwPlayerSerial == pOne->m_dwObjSerial)
    {
      return requestData;
    }
  }
  return nullptr;
}

bool GMCallMgr::RequestGMCall(CPlayer *pOne, int bCall)
{
  if (!pOne)
  {
    return false;
  }

  GMRequestData *requestData = GetGMRequestDataPtr(pOne);
  if (bCall)
  {
    if (!requestData)
    {
      GMRequestData *newRequest = PopReqEmptNode();
      if (newRequest)
      {
        newRequest->Set(pOne->m_dwObjSerial, pOne->m_pUserDB->m_wszAvatorName, timeGetTime());
        return SendResponseGMCall(pOne, 1) != 0;
      }
    }
  }
  else if (requestData)
  {
    PushReqNode(requestData);
    return SendResponseGMCall(pOne, 0) != 0;
  }

  return false;
}

bool GMCallMgr::RequestGMList(CPlayer *pOne, int nCurrPageIndex)
{
  return pOne && pOne->m_byUserDgr >= 2u && SendResponseGMList(pOne, nCurrPageIndex) != 0;
}

bool GMCallMgr::RequestAcceptGMCall(CPlayer *pOne, unsigned int dwUserSerial)
{
  int errorCode = 0;
  GMRequestData *requestData = nullptr;
  CPlayer *targetPlayer = nullptr;

  if (pOne && pOne->m_byUserDgr >= 2u)
  {
    targetPlayer = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, dwUserSerial);
    if (targetPlayer && targetPlayer->m_pCurMap && targetPlayer->m_pCurMap->m_pMapSet
      && !targetPlayer->m_pCurMap->m_pMapSet->m_nMapType)
    {
      requestData = GetGMRequestDataPtr(targetPlayer);
      if (requestData)
      {
        float newPos[8]{};
        targetPlayer->m_pCurMap->GetRandPosInRange(targetPlayer->m_fCurPos, 30, newPos);
        if (targetPlayer->m_pCurMap->IsMapIn(newPos))
        {
          if (pOne->OutOfMap(targetPlayer->m_pCurMap, 0, 4u, newPos))
          {
            pOne->SendMsg_GotoRecallResult(0, targetPlayer->m_pCurMap->m_pMapSet->m_dwIndex, newPos, 4u);
            RequestGMCall(targetPlayer, 0);
          }
          else
          {
            errorCode = 2;
          }
        }
        else
        {
          errorCode = 2;
        }
      }
      else
      {
        errorCode = 1;
      }
    }
    else
    {
      errorCode = 3;
    }
  }
  else
  {
    errorCode = 3;
  }

  if (errorCode)
  {
    SendResponseAcceptResult(pOne, targetPlayer, errorCode);
    return false;
  }

  SendResponseAcceptResult(pOne, targetPlayer, 0);
  return true;
}

bool GMCallMgr::SendResponseGMCall(CPlayer *pOne, int bCallState)
{
  if (!pOne)
  {
    return false;
  }

  _gm_msg_gmcall_response_zocl msg{};
  msg.m_bCall = bCallState;

  unsigned __int8 type[2]{};
  type[0] = 55;
  type[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pOne->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return true;
}

bool GMCallMgr::SendResponseGMList(CPlayer *pOne, int nCurrPageIndex)
{
  if (!pOne)
  {
    return false;
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
    226u);
  return true;
}

void GMCallMgr::SendResponseAcceptResult(CPlayer *pOneGM, CPlayer *pOneUser, int nErrorCode)
{
  (void)pOneUser;

  _gm_msg_gm_accept_response_zocl msg{};
  msg.m_nResultCode = nErrorCode;

  unsigned __int8 type[2]{};
  type[0] = 55;
  type[1] = 6;
  g_Network.m_pProcess[0]->LoadSendMsg(
    pOneGM->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

