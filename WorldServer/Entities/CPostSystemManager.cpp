#include "pch.h"

#include "CPostSystemManager.h"

#include <cstdarg>
#include <ctime>
#include <cstring>

#include "CMgrAvatorItemHistory.h"
#include "CMainThread.h"
#include "CPlayer.h"
#include "CPostReturnStorage.h"
#include "CPostStorage.h"
#include "CRFWorldDatabase.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "WorldServerUtil.h"
#include "qry_case_post_list_regi.h"
#include "qry_case_post_send.h"
#include "qry_case_post_serial_check.h"

#include <cstdio>
#include <atltime.h>

CPostSystemManager *CPostSystemManager::ms_Instance = nullptr;

CPostSystemManager::CPostSystemManager()
{
  m_byRegiNum = 20;
  m_byProcNum = 15;
  m_nPostProcCountPerDay = 0;
  m_nPostReturnCountPerDay = 0;
}

CPostSystemManager::~CPostSystemManager()
{
  if (m_pkLogger)
  {
    delete m_pkLogger;
    m_pkLogger = nullptr;
  }

  if (m_PostData)
  {
    delete[] m_PostData;
    m_PostData = nullptr;
  }
}

CPostSystemManager *CPostSystemManager::Instace()
{
  if (!ms_Instance)
  {
    ms_Instance = new CPostSystemManager();
  }

  return ms_Instance;
}

void CPostSystemManager::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

bool CPostSystemManager::InitLogger()
{
  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\PostSystem", nullptr);

  m_pkLogger = new CLogFile();
  if (m_pkLogger == nullptr)
  {
    g_Main.m_logLoadingError.Write(
      "CPostSystemManager::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  char path[128]{};
  const unsigned int now = GetKorLocalTime();
  sprintf_s(path, sizeof(path), "..\\ZoneServerLog\\Systemlog\\PostSystem\\PostSystem%u.log", now);
  m_pkLogger->SetWriteLogFile(path, 1, 0, 1, 1);
  return true;
}

bool CPostSystemManager::Init()
{
  if (!InitLogger())
  {
    return false;
  }

  m_PostData = new CPostData[500];
  if (m_PostData == nullptr)
  {
    return false;
  }

  m_tmrRegiTime.BeginTimer(1000);
  m_tmrProcTime.BeginTimer(30000);

  m_listEmpty.SetList(500);
  m_listRegist.SetList(500);
  m_listProc.SetList(500);

  for (unsigned int i = 0; i < 500; ++i)
  {
    m_listEmpty.PushNode_Back(i);
  }

  return true;
}

void CPostSystemManager::SetNextWriteTime()
{
  ATL::CTimeSpan oneDay(1, 0, 0, 0);
  ATL::CTime tomorrow = ATL::CTime::GetCurrentTime() + oneDay;
  ATL::CTime nextWriteTime(tomorrow.GetYear(), tomorrow.GetMonth(), tomorrow.GetDay(), 0, 0, 0, -1);
  m_tNextWrite = nextWriteTime.GetTime();
}

void CPostSystemManager::Loop()
{
  if (!g_Main.m_pWorldDB)
  {
    return;
  }

  if (m_tmrRegiTime.CountingTimer())
  {
    const int pendingRegistCount = m_listRegist.size();
    if (pendingRegistCount > 0)
    {
      _qry_case_post_list_regi query;
      for (int recordIndex = 0; recordIndex < pendingRegistCount && recordIndex < 20; ++recordIndex)
      {
        unsigned int outIndex[5]{};
        m_listRegist.PopNode_Front(outIndex);

        CPostData *postData = &m_PostData[outIndex[0]];
        query.List[recordIndex].dwIndex = outIndex[0];
        query.List[recordIndex].bySendRace = postData->m_bySendRace;
        query.List[recordIndex].bySenderDgr = postData->m_bySenderDgr;
        query.List[recordIndex].dwSenderSerial = postData->m_dwSenderSerial;
        query.List[recordIndex].key = postData->m_Key;
        query.List[recordIndex].dwDur = postData->m_dwDur;
        query.List[recordIndex].dwUpt = postData->m_dwUpt;
        query.List[recordIndex].lnUID = postData->m_lnUID;
        query.List[recordIndex].dwGold = postData->m_dwGold;
        strcpy_s(query.List[recordIndex].wszSendName, 17, postData->m_wszSendName);
        strcpy_s(query.List[recordIndex].wszRecvName, 17, postData->m_wszRecvName);
        strcpy_s(query.List[recordIndex].wszTitle, 21, postData->m_wszTitle);
        strcpy_s(query.List[recordIndex].wszContent, 201, postData->m_wszContent);
        ++query.dwCount;
      }

      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x4Du, reinterpret_cast<char *>(&query), static_cast<int>(query.size()));
    }
  }

  if (m_tmrProcTime.CountingTimer())
  {
    const int pendingProcCount = m_listProc.size();
    if (pendingProcCount > 0)
    {
      _qry_case_post_serial_check query;
      for (int recordIndex = 0; recordIndex < pendingProcCount && recordIndex < 15; ++recordIndex)
      {
        unsigned int outIndex[5]{};
        m_listProc.PopNode_Front(outIndex);

        CPostData *postData = &m_PostData[outIndex[0]];
        query.List[recordIndex].dwIndex = outIndex[0];
        query.List[recordIndex].bySenderDgr = postData->m_bySenderDgr;
        query.List[recordIndex].bySenderRace = postData->m_bySendRace;
        strcpy_s(query.List[recordIndex].wszRecvName, 17, postData->m_wszRecvName);
        if (postData->m_Key.IsFilled() || postData->m_dwGold || postData->m_bySenderDgr >= 2u)
        {
          query.List[recordIndex].bCheckDgr = true;
        }
        ++query.dwCount;
      }

      m_nPostProcCountPerDay += query.dwCount;
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x81u, reinterpret_cast<char *>(&query), static_cast<int>(query.size()));
    }
  }

  __int64 now = 0;
  _time64(&now);
  if (now > m_tNextWrite)
  {
    Log(
      "Post Use Count > Total Use: %d, Return Post: %d\r\n",
      m_nPostProcCountPerDay,
      m_nPostReturnCountPerDay);
    m_nPostProcCountPerDay = 0;
    m_nPostReturnCountPerDay = 0;
    SetNextWriteTime();
  }
}

bool CPostSystemManager::Load()
{
  if (!UpdateDisappearOwnerRecord())
  {
    return false;
  }

  if (InsertDefaultPSRecord())
  {
    return PostRegistryLoad() != 0;
  }

  return false;
}

char CPostSystemManager::UpdateDisappearOwnerRecord()
{
  if (g_Main.m_pWorldDB->Update_DisappearOwnerRecord())
  {
    return 1;
  }

  Log(
    "CPostSystemManager::UpdateDisappearOwner\r\n\t\tg_Main.m_pWorldDB->Update_DisappearOwnerRecord() Fail!\r\n");
  return 0;
}

char CPostSystemManager::InsertDefaultPSRecord()
{
  const int emptyCount = g_Main.m_pWorldDB->Select_PostStorageEmptyRecord();
  if (emptyCount >= 0)
  {
    if (emptyCount >= 5000 || g_Main.m_pWorldDB->Insert_PSDefaultRecord(0x1388u))
    {
      return 1;
    }

    Log(
      "CPostSystemManager::InsertDefaultPSRecord\r\n\t\tg_Main.m_pWorldDB->Insert_PSDefaultRecord( %d ) Fail!\r\n",
      5000);
    return 0;
  }

  Log(
    "CPostSystemManager::InsertDefaultPSRecord\r\n"
    "\t\tnCount(%d) = g_Main.m_pWorldDB->Select_PostStorageEmptyRecord() Fail!\r\n",
    emptyCount);
  return 0;
}

char CPostSystemManager::PostRegistryLoad()
{
  CPostData temp[500]{};
  const unsigned __int8 result = g_Main.m_pWorldDB->Select_PostRegistryData(0x1F4u, temp);
  if (result)
  {
    if (result == 2)
    {
      return 1;
    }
    return 0;
  }

  memcpy_s(m_PostData, sizeof(CPostData) * 500, temp, sizeof(CPostData) * 500);
  for (unsigned int dwIndex = 0; static_cast<int>(dwIndex) < 500; ++dwIndex)
  {
    if (!m_PostData[dwIndex].m_byState)
    {
      if (m_listEmpty.FindNode(dwIndex))
      {
        m_listRegist.PushNode_Back(dwIndex);
      }
    }
  }

  return 1;
}


void CPostSystemManager::Log(const char *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list va;
  va_start(va, fmt);
  m_pkLogger->WriteFromArg(fmt, va);
  va_end(va);
}

void CPostSystemManager::Log(const wchar_t *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list va;
  va_start(va, fmt);
  m_pkLogger->WriteFromArg(fmt, va);
  va_end(va);
}


unsigned __int8 CPostSystemManager::UpdateRegist(_qry_case_post_list_regi *pData)
{
  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_post_list_regi::__list &entry = pData->List[j];
    const int nK = static_cast<int>(entry.key.CovDBKey());
    entry.bRet = g_Main.m_pWorldDB->Update_PostRegistry(
      entry.dwIndex,
      entry.dwSenderSerial,
      entry.wszSendName,
      entry.wszRecvName,
      entry.wszTitle,
      entry.wszContent,
      nK,
      entry.dwDur,
      entry.dwUpt,
      entry.dwGold,
      entry.bySendRace,
      entry.bySenderDgr,
      entry.lnUID);
  }
  return 0;
}

unsigned __int8 CPostSystemManager::PostSend(_qry_case_post_send *pData)
{
  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_post_send::__list &entry = pData->List[j];
    if (!entry.byErr)
    {
      const int recvCount = g_Main.m_pWorldDB->Select_PostRecvStorageCheck(entry.dwReceiverSerial);
      if (recvCount >= 50)
      {
        entry.byErr = 10;
      }
    }

    unsigned __int8 errCode = 0;
    unsigned int ownerSerial = 0;
    if (entry.byErr)
    {
      errCode = 100;
      ownerSerial = entry.dwSenderSerial;
    }
    else
    {
      errCode = 0;
      ownerSerial = entry.dwReceiverSerial;
    }

    if (!g_Main.m_pWorldDB->Select_PostStorageRecordCheck())
    {
      g_Main.m_pWorldDB->Insert_PostStorageRecord();
    }

    unsigned __int8 number[36]{};
    if (g_Main.m_pWorldDB->Select_PostStorageEmptyRecordSerial(&entry.dwPSSerial))
    {
      const int nK = static_cast<int>(entry.key.CovDBKey());
      g_Main.m_pWorldDB->Update_PostStorageSendToRecver(
        ownerSerial,
        entry.dwPSSerial,
        errCode,
        entry.wszSendName,
        entry.wszRecvName,
        entry.wszTitle,
        entry.wszContent,
        nK,
        entry.dwDur,
        entry.dwUpt,
        entry.dwGold,
        entry.byErr,
        0xFFu,
        number,
        true,
        entry.lnUID);
    }
  }

  return 0;
}

unsigned __int8 CPostSystemManager::PostReceiverCheck(_qry_case_post_serial_check *pData)
{
  unsigned int outSerial[8]{};
  unsigned int accSerial[8]{};
  unsigned int fatigue[8]{};
  unsigned __int8 status[16]{};

  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    unsigned int race[5]{};
    _qry_case_post_serial_check::__list &entry = pData->List[j];
    g_Main.m_pWorldDB->Update_PostRegistryDisable(entry.dwIndex);
    unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_PostRecvSerialFromName(
      entry.wszRecvName,
      outSerial,
      accSerial,
      race);
    if (dbRet)
    {
      entry.byErr = (dbRet == 2) ? 9 : 8;
    }
    else
    {
      dbRet = g_Main.m_pWorldDB->Select_PlayerTimeLimitInfo(accSerial[0], fatigue, status);
      if (!dbRet || dbRet == 2)
      {
        bool invalid = false;
        if (entry.bCheckDgr)
        {
          if (entry.bySenderDgr >= 2u)
          {
            invalid = accSerial[0] < 0x77359400;
          }
          else
          {
            invalid = accSerial[0] >= 0x77359400;
          }
        }
        if (invalid)
        {
          entry.byErr = 8;
        }
        else if (status[0] == 99)
        {
          entry.byErr = 18;
        }
        else
        {
          if (entry.bySenderRace == race[0] / 2)
          {
            entry.dwReceiverSerial = outSerial[0];
          }
          else
          {
            entry.byErr = 13;
          }
        }
      }
      else
      {
        entry.byErr = 8;
      }
    }
  }

  return 0;
}

unsigned __int8 CPostSystemManager::CheckRegister(
  CPlayer *pOne,
  _STORAGE_POS_INDIV *pItemInfo,
  unsigned int dwGold,
  _STORAGE_LIST::_db_con **pItem)
{
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(pOne->m_id.wIndex) == 99)
  {
    return 17;
  }
  if (m_listEmpty.size() <= 0)
  {
    return 1;
  }

  const unsigned __int64 totalGold = static_cast<unsigned __int64>(dwGold) + 5u;
  if (totalGold > 0x7A120)
  {
    return 4;
  }

  const unsigned int curGold = pOne->m_Param.GetGold();
  if (totalGold > curGold)
  {
    return 4;
  }

  if (pItemInfo->byStorageCode >= 8u || pItemInfo->byStorageCode)
  {
    return 3;
  }
  if (pItemInfo->wItemSerial == 0xFFFF)
  {
    return 0;
  }

  _STORAGE_LIST *storage = pOne->m_Param.m_pStoragePtr[pItemInfo->byStorageCode];
  *pItem = storage->GetPtrFromSerial(pItemInfo->wItemSerial);
  if (!*pItem)
  {
    return 3;
  }
  if ((*pItem)->m_byTableCode == 19 || (*pItem)->m_bLock)
  {
    return 6;
  }
  if ((*pItem)->m_byCsMethod)
  {
    return 6;
  }
  if (!IsExchangeItem((*pItem)->m_byTableCode, (*pItem)->m_wItemIndex))
  {
    return 2;
  }
  if (!IsOverLapItem((*pItem)->m_byTableCode)
      || pItemInfo->byNum <= (*pItem)->m_dwDur)
  {
    return 0;
  }

  return 5;
}

char CPostSystemManager::PostSendRequest(
  CPlayer *pOne,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  _STORAGE_POS_INDIV *pItemInfo,
  unsigned int dwGold,
  unsigned __int8 byRace)
{
  _STORAGE_LIST::_db_con *item = nullptr;
  if (!pOne->m_bPostLoad)
  {
    pOne->SendMsg_PostSendReply(8u);
    return 0;
  }

  unsigned __int8 errCode = CheckRegister(pOne, pItemInfo, dwGold, &item);
  if (errCode != 17)
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *player = &g_Player[j];
      if (player
          && player->m_bOper
          && player->m_bLive
          && !strcmp_0(wszRecvName, player->m_pUserDB->m_AvatorData.dbAvator.m_wszAvatorName))
      {
        if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(player->m_id.wIndex) == 99)
        {
          errCode = 18;
        }
        break;
      }
    }
  }

  if (!IsSQLValidString(wszRecvName))
  {
    const char *name = pOne->m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "CPostSystemManager::PostSendRequest() : %u(%s) ::IsSQLValidString( wszRecvName(%s) ) Invalid!",
      pOne->m_dwObjSerial,
      name,
      wszRecvName);
    errCode = 16;
  }
  if (!IsSQLValidString(wszTitle))
  {
    const char *name = pOne->m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "CPostSystemManager::PostSendRequest() : %u(%s) ::IsSQLValidString( wszTitle(%s) ) Invalid!",
      pOne->m_dwObjSerial,
      name,
      wszTitle);
    errCode = 16;
  }
  if (!IsSQLValidString(wszContent))
  {
    const char *name = pOne->m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "CPostSystemManager::PostSendRequest() : %u(%s) ::IsSQLValidString( wszContent(%s) ) Invalid!",
      pOne->m_dwObjSerial,
      name,
      wszContent);
    errCode = 16;
  }

  if (!strlen_0(wszTitle) || !strlen_0(wszRecvName))
  {
    errCode = 8;
  }
  if (!strlen_0(wszContent))
  {
    strcpy_0(wszContent, " ");
  }

  if (!errCode)
  {
    int isOverlap = 0;
    _STORAGE_LIST::_db_con itemCopy;
    bool hasItem = false;
    if (item)
    {
      hasItem = true;
      itemCopy = *item;
      isOverlap = IsOverLapItem(itemCopy.m_byTableCode);
      if (isOverlap)
      {
        itemCopy.m_dwDur = pItemInfo->byNum;
        pOne->Emb_AlterDurPoint(0, itemCopy.m_byStorageIndex, -static_cast<int>(itemCopy.m_dwDur), false, false);
        itemCopy.m_lnUID = 0;
      }
      else if (!pOne->Emb_DelStorage(0, itemCopy.m_byStorageIndex, false, true, "CPostSystemManager::PostSendRequest()"))
      {
        pOne->SendMsg_PostSendReply(8u);
        return 0;
      }
    }

    pOne->SubGold(dwGold + 5u);
    pOne->SendMsg_AlterMoneyInform(0);

    if (hasItem || dwGold)
    {
      if (isOverlap || item)
      {
        CPlayer::s_MgrItemHistory.post_senditem(
          wszRecvName,
          &itemCopy,
          itemCopy.m_dwDur,
          dwGold,
          pOne->m_szItemHistoryFileName);
      }
      else
      {
        CPlayer::s_MgrItemHistory.post_senditem(
          wszRecvName,
          nullptr,
          0,
          dwGold,
          pOne->m_szItemHistoryFileName);
      }
    }

    _INVENKEY key;
    unsigned __int64 dwDur = 0;
    unsigned int dwUpt = 0xFFFFFFF;
    unsigned __int64 lnUID = 0;
    key.SetRelease();
    if (hasItem)
    {
      key.byTableCode = itemCopy.m_byTableCode;
      key.wItemIndex = itemCopy.m_wItemIndex;
      dwDur = itemCopy.m_dwDur;
      dwUpt = itemCopy.m_dwLv;
      lnUID = itemCopy.m_lnUID;
    }

    unsigned int outIndex[5]{};
    m_listEmpty.PopNode_Front(outIndex);
    m_PostData[outIndex[0]].SetState(0);
    CPostData *post = &m_PostData[outIndex[0]];
    post->SetPostData(
      outIndex[0],
      pOne->m_pUserDB->m_dwSerial,
      pOne->m_pUserDB->m_wszAvatorName,
      wszRecvName,
      wszTitle,
      key,
      dwDur,
      dwUpt,
      dwGold,
      0,
      byRace,
      pOne->m_byUserDgr);
    post->SetPostContent(wszContent);
    post->SetPostItemSerial(lnUID);
    m_listRegist.PushNode_Back(outIndex[0]);
  }

  if (errCode)
  {
    pOne->SendMsg_PostSendReply(errCode);
  }
  else
  {
    pOne->pc_UpdateDataForPostSend();
  }
  return 1;
}


void CPostSystemManager::CompleteRegist(_qry_case_post_list_regi *pData)
{
  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_post_list_regi::__list &entry = pData->List[j];
    if (entry.bRet)
    {
      m_listProc.PushNode_Back(entry.dwIndex);
    }
    else
    {
      const int nK = static_cast<int>(entry.key.CovDBKey());
      Log(
        "CPostSystemManager::CompleteRegist() Serial(%d),PostInx(%d),K(%d),D(%d),U(%d),Gold(%d)",
        entry.dwSenderSerial,
        entry.dwIndex,
        nK,
        entry.dwDur,
        entry.dwUpt,
        entry.dwGold);
      m_listEmpty.PushNode_Back(entry.dwIndex);
    }
  }
}

void CPostSystemManager::CompleteSend(_qry_case_post_send *pData)
{
  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_post_send::__list &entry = pData->List[j];
    if (entry.byErr)
    {
      ++m_nPostReturnCountPerDay;
      CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, entry.dwSenderSerial);
      if (player && player->m_bOper && player->m_bPostLoad)
      {
        CPostData *postData = &m_PostData[entry.dwIndex];
        CPostData *post = player->m_Param.m_ReturnPostStorage.AddReturnPost(
          entry.byErr,
          entry.dwPSSerial,
          0x64u,
          postData->m_wszRecvName,
          postData->m_wszTitle,
          postData->m_wszContent,
          postData->m_Key,
          postData->m_dwDur,
          postData->m_dwUpt,
          postData->m_dwGold,
          postData->m_lnUID);
        if (post)
        {
          if (post->m_Key.IsFilled() || post->m_dwGold)
          {
            CPlayer::s_MgrItemHistory.post_returnreceive(post, player->m_szItemHistoryFileName);
          }
          player->SendMsg_PostReturn(
            post->m_byErrCode,
            post->m_dwPSSerial,
            post->m_wszRecvName,
            post->m_wszTitle,
            post->m_wszContent,
            post->m_Key.byTableCode,
            post->m_Key.wItemIndex,
            post->m_dwDur,
            post->m_dwUpt,
            post->m_dwGold);
        }
        else
        {
          int size = player->m_Param.m_ReturnPostStorage.GetSize();
          Log(
            "CPostSystemManager::CompletePostReturn() :AddReturnPost: SenderSerial(%d),PSSize(%d)",
            entry.dwSenderSerial,
            size);
        }
      }
    }
    m_listEmpty.PushNode_Back(entry.dwIndex);
  }
}

void CPostSystemManager::CompletePostReceiverCheck(_qry_case_post_serial_check *pData)
{
  _qry_case_post_send qry;
  int nNumber[5]{};

  for (unsigned int j = 0; j < pData->dwCount; ++j)
  {
    _qry_case_post_serial_check::__list &entry = pData->List[j];
    CPostData *postData = &m_PostData[entry.dwIndex];
    bool handled = false;

    if (!entry.byErr)
    {
      CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, entry.dwReceiverSerial);
      if (player)
      {
        if (player->m_bPostLoad && player->m_bOper)
        {
          if (player->m_Param.m_PostStorage.GetSize() < 50)
          {
            int index = -1;
            nNumber[0] = static_cast<unsigned int>(-1);
            postData->SetState(2u);
            index = player->m_Param.m_PostStorage.AddNewPost(
              postData->m_dwSenderSerial,
              postData->m_wszSendName,
              postData->m_wszRecvName,
              postData->m_wszTitle,
              postData->m_wszContent,
              postData->m_Key,
              postData->m_dwDur,
              postData->m_dwUpt,
              postData->m_dwGold,
              0,
              postData->m_byState,
              nNumber,
              postData->m_lnUID);
            CPostData *post = player->m_Param.m_PostStorage.GetPostDataFromInx(index);
            int nK = static_cast<int>(post->m_Key.CovDBKey());
            player->m_pUserDB->SetNewDBPostData(
              index,
              0,
              post->m_nNumber,
              post->m_byState,
              post->m_wszSendName,
              post->m_wszRecvName,
              post->m_wszTitle,
              post->m_wszContent,
              nK,
              post->m_dwDur,
              post->m_dwUpt,
              post->m_dwGold,
              post->m_lnUID);
            if (post->m_Key.IsFilled() || post->m_dwGold)
            {
              CPlayer::s_MgrItemHistory.post_receive(post, player->m_szItemHistoryFileName);
            }
            bool hasGold = post->m_dwGold != 0;
            bool hasItem = post->m_Key.IsFilled();
            player->SendMsg_PostDelivery(
              nNumber[0],
              index,
              post->m_wszSendName,
              post->m_wszTitle,
              hasItem,
              hasGold,
              0);
            m_listEmpty.PushNode_Back(entry.dwIndex);
            handled = true;
          }
          else
          {
            entry.byErr = 10;
          }
        }
        else
        {
          entry.byErr = 8;
        }
      }
    }

    if (!handled)
    {
      qry.pushdata(
        entry.dwIndex,
        entry.byErr,
        entry.dwReceiverSerial,
        postData->m_dwSenderSerial,
        postData->m_wszSendName,
        postData->m_wszRecvName,
        postData->m_wszTitle,
        postData->m_wszContent,
        postData->m_Key,
        postData->m_dwDur,
        postData->m_dwUpt,
        postData->m_dwGold,
        postData->m_lnUID);
    }
  }

  if (qry.dwCount)
  {
    int size = static_cast<int>(qry.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x4Eu, reinterpret_cast<char *>(&qry), size);
  }
}
