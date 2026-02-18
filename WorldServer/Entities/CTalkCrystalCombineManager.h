#pragma once

#include "IdaCompat.h"

#include "StorageList.h"
#include "talik_crystal_exchange_clzo.h"
#include "talik_crystal_exchange_zocl.h"

class CPlayer;

struct  _talk_crystal_matrial_combine_node
{
  struct  _matrialinfo
  {
    _STORAGE_LIST::_db_con *m_pMatrial;
    unsigned __int8 m_byConsume;
    unsigned __int8 m_byUseCount;
    unsigned __int8 m_byClientIndex;

    _matrialinfo();
    void Init();
  };

  int m_nMatrialCount;
  int m_nMatrialOverlapCount;
  int m_nMakeCount;
  bool m_bUse;
  int m_nMixIndex;
  int m_nNeedItemNum;
  unsigned __int8 m_byTableCode;
  unsigned __int16 m_wItemIndex;
  int m_nRequiredSlotCount;
  _matrialinfo m_matrialList[24];
  _STORAGE_LIST::_db_con m_MakeItem;

  _talk_crystal_matrial_combine_node();
  void Init();
  char Set(int nMixIndex, int nNeedItemNum, unsigned __int8 byTableCode, unsigned __int16 wItemIndex);
  char Push(_STORAGE_LIST::_db_con *pItem, unsigned __int8 byUseCount, unsigned __int8 byClientIndex);
  void Consume(int nConsumeCount);
  void Make(int nMakeCount);
  int GetMixNeedNum();
  int GetRequiredSlotCount();
};

class  CTalkCrystalCombineManager
{
public:
  static CTalkCrystalCombineManager *Instance();
  static void Destory();

  CTalkCrystalCombineManager();
  void Init();
  char Doit(CPlayer *pPlayer, unsigned __int8 byExchangeNum, _talik_crystal_exchange_clzo::_list *pList);
  unsigned __int8 CombinePreProcess(CPlayer *pPlayer, unsigned __int8 byExchangeNum, _talik_crystal_exchange_clzo::_list *pList);
  unsigned __int8 CombineProcess();
  unsigned __int8 CheckMixItem(
    _STORAGE_LIST::_db_con *pItem,
    int *pMixIndex,
    unsigned __int8 *pbyTableCode,
    unsigned __int16 *pwItemIndex,
    int *pnNeedItemCount);
  unsigned __int8 Push(_STORAGE_LIST::_db_con *pItem, unsigned __int8 byUseCount, unsigned __int8 byClientIndex);
  _talk_crystal_matrial_combine_node *MakeMixNode(
    int nMixIndex,
    int nNeedItemNum,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex);
  _talk_crystal_matrial_combine_node *GetMixNode(int nMixIndex);

  static CTalkCrystalCombineManager *ms_pInstance;

  _talk_crystal_matrial_combine_node m_NodeList[24];
  CPlayer *m_pCurrentPlayer;
};
