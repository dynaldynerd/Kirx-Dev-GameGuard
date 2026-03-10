#include "pch.h"

#include "CTalkCrystalCombineManager.h"

#include "CPlayer.h"
#include "CRecordData.h"
#include "ResourceItem_fld.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CTalkCrystalCombineManager *CTalkCrystalCombineManager::ms_pInstance;

_talk_crystal_matrial_combine_node::_matrialinfo::_matrialinfo()
{
  Init();
}

void _talk_crystal_matrial_combine_node::_matrialinfo::Init()
{
  m_pMatrial = nullptr;
  m_byConsume = 0;
  m_byUseCount = 0;
  m_byClientIndex = static_cast<unsigned __int8>(-1);
}

_talk_crystal_matrial_combine_node::_talk_crystal_matrial_combine_node()
{
  Init();
}

void _talk_crystal_matrial_combine_node::Init()
{
  m_nMatrialCount = 0;
  m_nMatrialOverlapCount = 0;
  m_nMakeCount = 0;
  m_bUse = false;
  m_nMixIndex = -1;
  m_nNeedItemNum = 0;
  m_byTableCode = static_cast<unsigned __int8>(-1);
  m_wItemIndex = static_cast<unsigned __int16>(-1);
  m_nRequiredSlotCount = 0;
  memset_0(&m_MakeItem, 0, sizeof(m_MakeItem));
  for (int j = 0; j < 24; ++j)
  {
    m_matrialList[j].Init();
  }
}

char _talk_crystal_matrial_combine_node::Set(
  int nMixIndex,
  int nNeedItemNum,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex)
{
  Init();
  m_nMixIndex = nMixIndex;
  m_bUse = true;
  m_nNeedItemNum = nNeedItemNum;
  m_byTableCode = byTableCode;
  m_wItemIndex = wItemIndex;

  void *src = MakeLoot(m_byTableCode, m_wItemIndex);
  if (!src)
  {
    return 0;
  }

  memcpy_0(&m_MakeItem, src, sizeof(m_MakeItem));
  return 1;
}

char _talk_crystal_matrial_combine_node::Push(
  _STORAGE_LIST::_db_con *pItem,
  unsigned __int8 byUseCount,
  unsigned __int8 byClientIndex)
{
  if (m_nMatrialCount >= 24)
  {
    return 0;
  }

  m_matrialList[m_nMatrialCount].m_pMatrial = pItem;
  m_matrialList[m_nMatrialCount].m_byUseCount = byUseCount;
  m_matrialList[m_nMatrialCount].m_byClientIndex = byClientIndex;
  m_nMatrialOverlapCount += byUseCount;
  ++m_nMatrialCount;
  return 1;
}

void _talk_crystal_matrial_combine_node::Consume(int nConsumeCount)
{
  for (int j = 0; j < m_nMatrialCount; ++j)
  {
    if (nConsumeCount <= 0)
    {
      continue;
    }

    if (nConsumeCount < m_matrialList[j].m_byUseCount)
    {
      m_matrialList[j].m_byConsume = static_cast<unsigned __int8>(nConsumeCount);
      nConsumeCount = 0;
    }
    else
    {
      m_matrialList[j].m_byConsume = m_matrialList[j].m_byUseCount;
      nConsumeCount -= m_matrialList[j].m_byConsume;
    }
  }
}

void _talk_crystal_matrial_combine_node::Make(int nMakeCount)
{
  m_nMakeCount = nMakeCount;
  if (IsOverLapItem(m_MakeItem.m_byTableCode))
  {
    m_nRequiredSlotCount = m_nMakeCount / MAX_ITEM_OVERLAP;
    if (m_nMakeCount % MAX_ITEM_OVERLAP > 0)
    {
      ++m_nRequiredSlotCount;
    }
  }
  else
  {
    m_nRequiredSlotCount = nMakeCount;
  }
}

int _talk_crystal_matrial_combine_node::GetMixNeedNum()
{
  return m_nNeedItemNum;
}

int _talk_crystal_matrial_combine_node::GetRequiredSlotCount()
{
  return m_nRequiredSlotCount;
}

CTalkCrystalCombineManager::CTalkCrystalCombineManager()
{
  Init();
}

CTalkCrystalCombineManager *CTalkCrystalCombineManager::Instance()
{
  if (!ms_pInstance)
  {
    ms_pInstance = new CTalkCrystalCombineManager();
  }
  return ms_pInstance;
}

void CTalkCrystalCombineManager::Destory()
{
  if (ms_pInstance)
  {
    delete ms_pInstance;
    ms_pInstance = nullptr;
  }
}

void CTalkCrystalCombineManager::Init()
{
  m_pCurrentPlayer = nullptr;
  for (int j = 0; j < 24; ++j)
  {
    m_NodeList[j].Init();
  }
}

unsigned __int8 CTalkCrystalCombineManager::CheckMixItem(
  _STORAGE_LIST::_db_con *pItem,
  int *pMixIndex,
  unsigned __int8 *pbyTableCode,
  unsigned __int16 *pwItemIndex,
  int *pnNeedItemCount)
{
  if (!m_pCurrentPlayer || !pItem || !pMixIndex || !pnNeedItemCount || !pwItemIndex || !pbyTableCode)
  {
    return 4;
  }
  if (pItem->m_byTableCode != 18)
  {
    return 12;
  }

  _ResourceItem_fld *record = static_cast<_ResourceItem_fld *>(g_Main.m_tblItemData[18].GetRecord(pItem->m_wItemIndex));
  if (!record)
  {
    return 4;
  }

  if (record->m_nMixNum < 0)
  {
    return 16;
  }

  *pMixIndex = record->m_nMixNum;
  const int itemTableCode = GetItemTableCode(record->m_strLastItem);
  if (itemTableCode == -1)
  {
    return 8;
  }

  _base_fld *recordByHash = g_Main.m_tblItemData[itemTableCode].GetRecordByHash(record->m_strLastItem, 2, 5);
  if (!recordByHash)
  {
    return 8;
  }

  *pbyTableCode = static_cast<unsigned __int8>(itemTableCode);
  *pwItemIndex = static_cast<unsigned __int16>(recordByHash->m_dwIndex);
  *pnNeedItemCount = static_cast<int>(record->m_fNeedNum);
  if (*pnNeedItemCount >= 0)
  {
    return 0;
  }

  return 16;
}

unsigned __int8 CTalkCrystalCombineManager::Push(
  _STORAGE_LIST::_db_con *pItem,
  unsigned __int8 byUseCount,
  unsigned __int8 byClientIndex)
{
  int mixIndex = -1;
  unsigned __int8 tableCode = static_cast<unsigned __int8>(-1);
  unsigned __int16 itemIndex = static_cast<unsigned __int16>(-1);
  int needItemNum = -1;

  const unsigned __int8 err = CheckMixItem(pItem, &mixIndex, &tableCode, &itemIndex, &needItemNum);
  if (err)
  {
    return err;
  }

  _talk_crystal_matrial_combine_node *mixNode = GetMixNode(mixIndex);
  if (!mixNode)
  {
    mixNode = MakeMixNode(mixIndex, needItemNum, tableCode, itemIndex);
  }
  if (!mixNode)
  {
    return 12;
  }

  if (mixNode->Push(pItem, byUseCount, byClientIndex))
  {
    return 0;
  }

  return 12;
}

unsigned __int8 CTalkCrystalCombineManager::CombinePreProcess(
  CPlayer *pPlayer,
  unsigned __int8 byExchangeNum,
  _talik_crystal_exchange_clzo::_list *pList)
{
Init();
  m_pCurrentPlayer = pPlayer;

  const unsigned __int8 maxCount = 24;
  for (unsigned __int8 j = 0; j < maxCount; ++j)
  {
    if (!pList[j].byItemCount)
    {
      continue;
    }

    for (unsigned __int8 k = static_cast<unsigned __int8>(j + 1); k < maxCount; ++k)
    {
      if (pList[k].byItemCount && pList[k].wSerial == pList[j].wSerial)
      {
        return 5;
      }
    }

    _STORAGE_LIST::_db_con *item = pPlayer->m_Param.m_dbInven.GetPtrFromSerial(pList[j].wSerial);
    if (!item)
    {
      return 4;
    }
    if (item->m_byTableCode != 18)
    {
      return 4;
    }

    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur < pList[j].byItemCount)
      {
        return 5;
      }
    }
    else if (pList[j].byItemCount != 1)
    {
      return 5;
    }

    const unsigned __int8 err = Push(item, pList[j].byItemCount, j);
    if (err)
    {
      return err;
    }
  }

  return 0;
}

unsigned __int8 CTalkCrystalCombineManager::CombineProcess()
{
  int requiredSlotCount = 0;

  for (int j = 0; j < 24; ++j)
  {
    _talk_crystal_matrial_combine_node *node = &m_NodeList[j];
    if (!node->m_bUse)
    {
      continue;
    }

    const int mixNeedNum = node->GetMixNeedNum();
    const int remain = node->m_nMatrialOverlapCount % mixNeedNum;
    const int makeCount = node->m_nMatrialOverlapCount / mixNeedNum;
    const int consumeCount = node->m_nMatrialOverlapCount - remain;
    node->Consume(consumeCount);
    node->Make(makeCount);
    requiredSlotCount += node->GetRequiredSlotCount();
  }

  const int indexEmptyCon = m_pCurrentPlayer->m_Param.m_dbInven.GetIndexEmptyCon();
  if (indexEmptyCon == 255)
  {
    return 3;
  }

  if (requiredSlotCount <= indexEmptyCon)
  {
    return 0;
  }

  return 3;
}

char CTalkCrystalCombineManager::Doit(
  CPlayer *pPlayer,
  unsigned __int8 byExchangeNum,
  _talik_crystal_exchange_clzo::_list *pList)
{
  _talik_crystal_exchange_zocl send{};
  unsigned __int8 pbyType[2]{7, 76};

  unsigned __int8 errCode = CombinePreProcess(pPlayer, byExchangeNum, pList);
  if (!errCode)
  {
    errCode = CombineProcess();
  }
  if (errCode)
  {
    send.byErrorCode = errCode;
    const unsigned __int16 nLen = send.size();
    g_Network.m_pProcess[0]->LoadSendMsg(pPlayer->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&send), nLen);
    return 0;
  }

  int makeNodeCount = 0;
  for (int j = 0; j < 24; ++j)
  {
    _talk_crystal_matrial_combine_node *node = &m_NodeList[j];
    if (!node->m_bUse)
    {
      continue;
    }

    for (int k = 0; k < node->m_nMatrialCount; ++k)
    {
      _talk_crystal_matrial_combine_node::_matrialinfo *material = &node->m_matrialList[k];
      const unsigned __int8 clientIndex = material->m_byClientIndex;
      send.RemainItem[clientIndex].byRemainNum = material->m_byUseCount - material->m_byConsume;
      pPlayer->Emb_AlterDurPoint(0, material->m_pMatrial->m_byStorageIndex, -material->m_byConsume, true, false);
    }

    if (node->m_nMakeCount <= 0)
    {
      continue;
    }

    if (IsOverLapItem(node->m_MakeItem.m_byTableCode))
    {
      node->m_MakeItem.m_dwDur = static_cast<unsigned int>(node->m_nMakeCount);
      const unsigned __int8 fullStack =
        static_cast<unsigned __int8>(node->m_MakeItem.m_dwDur / MAX_ITEM_OVERLAP);
      const unsigned __int8 remainStack =
        static_cast<unsigned __int8>(node->m_MakeItem.m_dwDur % MAX_ITEM_OVERLAP);

      for (unsigned __int8 m = 0; m < fullStack; ++m)
      {
        node->m_MakeItem.m_dwDur = MAX_ITEM_OVERLAP;
        node->m_MakeItem.m_wSerial = pPlayer->m_Param.GetNewItemSerial();
        pPlayer->Emb_AddStorage(0, &node->m_MakeItem, 0, 1);
        pPlayer->SendMsg_RewardAddItem(&node->m_MakeItem, 0);
      }

      if (remainStack)
      {
        node->m_MakeItem.m_dwDur = remainStack;
        node->m_MakeItem.m_wSerial = pPlayer->m_Param.GetNewItemSerial();
        pPlayer->Emb_AddStorage(0, &node->m_MakeItem, 0, 1);
        pPlayer->SendMsg_RewardAddItem(&node->m_MakeItem, 0);
      }
    }
    else
    {
      for (int n = 0; n < node->m_nMakeCount; ++n)
      {
        node->m_MakeItem.m_dwDur = GetItemDurPoint(node->m_MakeItem.m_byTableCode, node->m_MakeItem.m_wItemIndex);
        node->m_MakeItem.m_wSerial = pPlayer->m_Param.GetNewItemSerial();
        pPlayer->Emb_AddStorage(0, &node->m_MakeItem, 0, 1);
        pPlayer->SendMsg_RewardAddItem(&node->m_MakeItem, 0);
      }
    }

    ++makeNodeCount;
  }

  if (makeNodeCount <= 0)
  {
    send.byErrorCode = 17;
  }
  else
  {
    send.byErrorCode = 0;
  }

  const unsigned __int16 nLen = send.size();
  g_Network.m_pProcess[0]->LoadSendMsg(pPlayer->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&send), nLen);
  return 1;
}

_talk_crystal_matrial_combine_node *CTalkCrystalCombineManager::MakeMixNode(
  int nMixIndex,
  int nNeedItemNum,
  unsigned __int8 byTableCode,
  unsigned __int16 wItemIndex)
{
  int freeIndex = 0;
  for (; freeIndex < 24; ++freeIndex)
  {
    if (!m_NodeList[freeIndex].m_bUse)
    {
      break;
    }
  }
  if (freeIndex >= 24)
  {
    return nullptr;
  }

  if (m_NodeList[freeIndex].Set(nMixIndex, nNeedItemNum, byTableCode, wItemIndex))
  {
    return &m_NodeList[freeIndex];
  }

  return nullptr;
}

_talk_crystal_matrial_combine_node *CTalkCrystalCombineManager::GetMixNode(int nMixIndex)
{
  for (int j = 0; j < 24; ++j)
  {
    if (m_NodeList[j].m_bUse && m_NodeList[j].m_nMixIndex == nMixIndex)
    {
      return &m_NodeList[j];
    }
  }

  return nullptr;
}
