#include "pch.h"

#include "CPostSystemManager.h"

#include <cstdarg>
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
#include "qry_case_post_send.h"

#include <cstdio>

CPostSystemManager *CPostSystemManager::Instace()
{
  static CPostSystemManager s_instance;
  return &s_instance;
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


unsigned __int8 CPostSystemManager::UpdateRegist(char *pData)
{
  for (unsigned int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = &pData[304 * j + 8];
    int nK = static_cast<int>(reinterpret_cast<_INVENKEY *>(pData + 304 * j + 276)->CovDBKey());
    bool updated = g_Main.m_pWorldDB->Update_PostRegistry(
      *reinterpret_cast<unsigned int *>(entry),
      *reinterpret_cast<unsigned int *>(entry + 8),
      entry + 12,
      entry + 29,
      entry + 46,
      entry + 67,
      nK,
      *reinterpret_cast<unsigned long long *>(entry + 272),
      *reinterpret_cast<unsigned int *>(entry + 280),
      *reinterpret_cast<unsigned int *>(entry + 296),
      static_cast<unsigned __int8>(entry[5]),
      static_cast<unsigned __int8>(entry[6]),
      *reinterpret_cast<unsigned long long *>(entry + 288));
    entry[4] = updated ? 1 : 0;
  }
  return 0;
}

unsigned __int8 CPostSystemManager::PostSend(char *pData)
{
  for (unsigned int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = &pData[312 * j + 8];
    if (!entry[0])
    {
      int recvCount = g_Main.m_pWorldDB->Select_PostRecvStorageCheck(*reinterpret_cast<unsigned int *>(entry + 12));
      if (recvCount >= 50)
      {
        entry[0] = 10;
      }
    }

    unsigned __int8 errCode = 0;
    unsigned int ownerSerial = 0;
    if (entry[0])
    {
      errCode = 100;
      ownerSerial = *reinterpret_cast<unsigned int *>(entry + 16);
    }
    else
    {
      errCode = 0;
      ownerSerial = *reinterpret_cast<unsigned int *>(entry + 12);
    }

    if (!g_Main.m_pWorldDB->Select_PostStorageRecordCheck())
    {
      g_Main.m_pWorldDB->Insert_PostStorageRecord();
    }

    unsigned __int8 number[36]{};
    if (g_Main.m_pWorldDB->Select_PostStorageEmptyRecordSerial(reinterpret_cast<unsigned int *>(entry + 8)))
    {
      int nK = static_cast<int>(reinterpret_cast<_INVENKEY *>(entry + 276)->CovDBKey());
      g_Main.m_pWorldDB->Update_PostStorageSendToRecver(
        ownerSerial,
        *reinterpret_cast<unsigned int *>(entry + 8),
        errCode,
        entry + 20,
        entry + 37,
        entry + 54,
        entry + 75,
        nK,
        *reinterpret_cast<unsigned long long *>(entry + 280),
        *reinterpret_cast<unsigned int *>(entry + 288),
        *reinterpret_cast<unsigned int *>(entry + 304),
        static_cast<unsigned __int8>(entry[0]),
        0xFFu,
        number,
        true,
        *reinterpret_cast<unsigned long long *>(entry + 296));
    }
  }

  return 0;
}

unsigned __int8 CPostSystemManager::PostReceiverCheck(char *pData)
{
  unsigned int outSerial[8]{};
  unsigned int accSerial[8]{};
  unsigned int fatigue[8]{};
  unsigned __int8 status[16]{};

  for (unsigned int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    unsigned int race[5]{};
    char *entry = &pData[32 * j + 4];
    g_Main.m_pWorldDB->Update_PostRegistryDisable(*reinterpret_cast<unsigned int *>(entry));
    unsigned __int8 dbRet = g_Main.m_pWorldDB->Select_PostRecvSerialFromName(
      entry + 7,
      outSerial,
      accSerial,
      race);
    if (dbRet)
    {
      entry[28] = (dbRet == 2) ? 9 : 8;
    }
    else
    {
      dbRet = g_Main.m_pWorldDB->Select_PlayerTimeLimitInfo(accSerial[0], fatigue, status);
      if (!dbRet || dbRet == 2)
      {
        bool invalid = false;
        if (entry[6])
        {
          if (static_cast<unsigned __int8>(entry[4]) >= 2u)
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
          entry[28] = 8;
        }
        else if (status[0] == 99)
        {
          entry[28] = 18;
        }
        else
        {
          if (static_cast<unsigned __int8>(entry[5]) == race[0] / 2)
          {
            *reinterpret_cast<unsigned int *>(entry + 24) = outSerial[0];
          }
          else
          {
            entry[28] = 13;
          }
        }
      }
      else
      {
        entry[28] = 8;
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


void CPostSystemManager::CompleteRegist(char *pData)
{
  for (unsigned int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = &pData[304 * j + 8];
    if (pData[304 * j + 12])
    {
      m_listProc.PushNode_Back(*reinterpret_cast<unsigned int *>(entry));
    }
    else
    {
      int nK = static_cast<int>(reinterpret_cast<_INVENKEY *>(entry + 268)->CovDBKey());
      Log(
        "CPostSystemManager::CompleteRegist() Serial(%d),PostInx(%d),K(%d),D(%d),U(%d),Gold(%d)",
        *reinterpret_cast<unsigned int *>(entry + 8),
        *reinterpret_cast<unsigned int *>(entry),
        nK,
        *reinterpret_cast<unsigned long long *>(entry + 272),
        *reinterpret_cast<unsigned int *>(entry + 280),
        *reinterpret_cast<unsigned int *>(entry + 296));
      m_listEmpty.PushNode_Back(*reinterpret_cast<unsigned int *>(entry));
    }
  }
}

void CPostSystemManager::CompleteSend(char *pData)
{
  for (unsigned int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = &pData[312 * j + 8];
    if (entry[0])
    {
      ++m_nPostReturnCountPerDay;
      CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(entry + 16));
      if (player && player->m_bOper && player->m_bPostLoad)
      {
        CPostData *postData = &m_PostData[*reinterpret_cast<unsigned int *>(entry + 4)];
        CPostData *post = player->m_Param.m_ReturnPostStorage.AddReturnPost(
          static_cast<unsigned __int8>(entry[0]),
          *reinterpret_cast<unsigned int *>(entry + 8),
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
            *reinterpret_cast<unsigned int *>(entry + 16),
            size);
        }
      }
    }
    m_listEmpty.PushNode_Back(*reinterpret_cast<unsigned int *>(entry + 4));
  }
}

void CPostSystemManager::CompletePostReceiverCheck(char *pData)
{
  _qry_case_post_send qry;
  int nNumber[5]{};

  for (unsigned int j = 0; j < *reinterpret_cast<unsigned int *>(pData); ++j)
  {
    char *entry = &pData[32 * j + 4];
    CPostData *postData = &m_PostData[*reinterpret_cast<unsigned int *>(entry)];
    bool handled = false;

    if (!pData[32 * j + 32])
    {
      CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(entry + 24));
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
            m_listEmpty.PushNode_Back(*reinterpret_cast<unsigned int *>(entry));
            handled = true;
          }
          else
          {
            entry[28] = 10;
          }
        }
        else
        {
          entry[28] = 8;
        }
      }
    }

    if (!handled)
    {
      qry.pushdata(
        *reinterpret_cast<unsigned int *>(entry),
        static_cast<unsigned __int8>(entry[28]),
        *reinterpret_cast<unsigned int *>(entry + 24),
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
