#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"
#include "CLogFile.h"

class CGuild;

struct _AUTOMINE_BATTERY
{
  int m_nCurGage;
  int m_nMaxGage;
};

template <typename T>
struct TInvenSlot
{
  T m_Item;
  int m_nOverlapNum;
  int m_nMaxOverlapNum;

  void clear();
};

template <typename T>
struct TInvenPage
{
  int m_nMaxSlotNum;
  int m_nMaxOverlapNum;
  TInvenSlot<T> *m_pSlot;

  void clear();
};

template <typename T>
struct TInventory
{
  int m_nUsingPage;
  int m_nMaxPageNum;
  int m_nMaxSlotNum;
  int m_nMaxOverlapNum;
  TInvenPage<T> *m_pPage;

  TInventory();
  ~TInventory() = default;

  void clear();
};

template <typename T>
TInventory<T>::TInventory()
  : m_nUsingPage(0),
    m_nMaxPageNum(0),
    m_nMaxSlotNum(0),
    m_nMaxOverlapNum(0),
    m_pPage(nullptr)
{
}

template <typename T>
void TInvenSlot<T>::clear()
{
  memset_0(&m_Item, 0, sizeof(m_Item));
  m_nOverlapNum = 0;
}

template <typename T>
void TInvenPage<T>::clear()
{
  for (int j = 0; j < m_nMaxSlotNum; ++j)
  {
    m_pSlot[j].clear();
  }
}

template <typename T>
void TInventory<T>::clear()
{
  for (int j = 0; j < m_nMaxPageNum; ++j)
  {
    m_pPage[j].clear();
  }
}

struct _pt_automine_charge_money_db_update_fail_zocl
{
  int nCharge;

  __int64 size();
};

class AutoMineMachine
{
public:
  AutoMineMachine();
  ~AutoMineMachine();

  void SubChargeCost(unsigned __int8 byRet, char *pdata);
  void ChangeOwner(CGuild *pOwnerGuild);
  void push_dqs_newowner();

  bool m_bInit;
  bool m_bOpenUI;
  bool m_bRunning;
  unsigned __int8 m_byRace;
  unsigned __int8 m_byCollisionType;
  unsigned __int8 m_bySelectedOre;
  _INVENKEY m_OreKind[5];
  CGuild *m_pOwnerGuild;
  unsigned int m_dwMiningTime;
  unsigned int m_dwMiningTerm;
  unsigned int m_dwDQSPushTime;
  _AUTOMINE_BATTERY m_Battery;
  TInventory<_INVENKEY> m_Inven;
  CLogFile m_sysLog;
  CLogFile m_Log;
};

class AutoMineMachineMng
{
public:
  AutoMineMachineMng();
  ~AutoMineMachineMng() = default;

  static AutoMineMachineMng *Instance();

  unsigned __int8 request_db_query(char *pdata);
  void result_db_query(unsigned __int8 byRet, char *pdata);
  AutoMineMachine *GetMachine(unsigned __int8 byRace, unsigned __int8 byCollisionType);
  void ChangeOwner(int nRaceCode, CGuild *pGuild, unsigned __int8 byCollisionType);

private:
  unsigned __int8 _db_qry_insert_newowner(char *pdata);
  unsigned __int8 _db_qry_update_battery_charge(char *pdata);
  unsigned __int8 _db_qry_update_mineore(char *pdata);
  unsigned __int8 _db_qry_update_workstate(char *pdata);
  unsigned __int8 _db_qry_update_selore(char *pdata);
  unsigned __int8 _db_qry_update_battery_discharge(char *pdata);
  unsigned __int8 _db_qry_update_moveore(char *pdata);

  AutoMineMachine m_Machine[3][2];

  static AutoMineMachineMng *m_pInstance;
};
