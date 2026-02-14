#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"
#include "CLogFile.h"

#include <new>

class CGuild;
class CPlayer;
struct _DB_LOAD_AUTOMINE_MACHINE;
struct _qry_case_amine_newowner;
struct _qry_case_amine_batterycharge;
struct _qry_case_amine_mineore;
struct _qry_case_amine_workstate;
struct _qry_case_amine_selore;
struct _qry_case_amine_battery_discharge;
struct _qry_case_amine_moveore;

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

  TInvenSlot();
  TInvenSlot(const TInvenSlot &rhs);
  ~TInvenSlot() = default;

  TInvenSlot &operator=(const TInvenSlot &rhs);

  void clear();
  bool is_empty() const;
  T *get_pitem();
  int get_overlapnum() const;
  void set_overlapnum(int n);
  bool able_overlap(const T *pItem, int nNum) const;
  int push(const T *pItem, unsigned int nNum);
  int pop(const T *pItem, int nNum);
};

template <typename T>
struct TInvenPage
{
  int m_nMaxSlotNum;
  int m_nMaxOverlapNum;
  TInvenSlot<T> *m_pSlot;

  TInvenPage();
  ~TInvenPage();

  bool create(int nMaxSlotNum, int nMaxOverlapNum);
  TInvenSlot<T> *get_slot(int n);
  int find_pos_empty();
  int find_pos_overlap(const T *pItem, int nNum);
  int find_empty(const T *pItem, int nNum);
  int push_overlap(const T *pItem, int nNum, int nS);
  int push_normal(const T *pItem, int nS);
  int push(const T *pItem, int nS, int nNum);
  int pop(const T *pItem, int nS, int nNum);
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

  bool create(int nMaxPage, int nMaxSlot, int nMaxOverlap);
  TInvenSlot<T> *get_slot(int nP, int nS);
  bool find_empty(const T *pItem, int nNum, int *nP, int *nS);
  int push(int nP, int nS, const T *pItem, int nNum);
  int pop(int nP, int nS, const T *pItem, int nNum);
  void swap(TInvenSlot<T> *pSrc, TInvenSlot<T> *pDst);
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
  memset_0(this, 0, sizeof(T));
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

template <typename T>
TInvenSlot<T>::TInvenSlot()
  : m_nOverlapNum(0),
    m_nMaxOverlapNum(99)
{
  memset_0(this, 0, sizeof(T));
}

template <typename T>
TInvenSlot<T>::TInvenSlot(const TInvenSlot &rhs)
  : m_nOverlapNum(rhs.m_nOverlapNum),
    m_nMaxOverlapNum(rhs.m_nMaxOverlapNum)
{
  m_Item = rhs.m_Item;
}

template <typename T>
TInvenSlot<T> &TInvenSlot<T>::operator=(const TInvenSlot &rhs)
{
  m_Item = rhs.m_Item;
  m_nOverlapNum = rhs.m_nOverlapNum;
  m_nMaxOverlapNum = rhs.m_nMaxOverlapNum;
  return *this;
}

template <typename T>
bool TInvenSlot<T>::is_empty() const
{
  return m_nOverlapNum == 0;
}

template <typename T>
T *TInvenSlot<T>::get_pitem()
{
  return m_nOverlapNum ? &m_Item : nullptr;
}

template <typename T>
int TInvenSlot<T>::get_overlapnum() const
{
  return m_nOverlapNum;
}

template <typename T>
void TInvenSlot<T>::set_overlapnum(int n)
{
  m_nOverlapNum = n;
}

template <typename T>
bool TInvenSlot<T>::able_overlap(const T *pItem, int nNum) const
{
  return pItem && m_Item == *pItem && m_nMaxOverlapNum >= nNum + m_nOverlapNum;
}

template <typename T>
int TInvenSlot<T>::push(const T *pItem, unsigned int nNum)
{
  if (!pItem)
  {
    return static_cast<int>(nNum);
  }

  if (static_cast<int>(nNum) > m_nMaxOverlapNum)
  {
    return static_cast<int>(nNum);
  }

  if (m_Item == *pItem)
  {
    if (m_Item.IsOverlapItem())
    {
      const int sum = static_cast<int>(nNum) + m_nOverlapNum;
      const int newOverlap = (sum <= m_nMaxOverlapNum) ? sum : m_nOverlapNum;
      m_nOverlapNum = newOverlap;
      return sum - m_nOverlapNum;
    }
    return 1;
  }

  memcpy_0(&m_Item, pItem, sizeof(T));
  m_nOverlapNum = static_cast<int>(nNum);
  return 0;
}

template <typename T>
int TInvenSlot<T>::pop(const T *pItem, int nNum)
{
  if (!pItem || !(m_Item == *pItem))
  {
    return -1;
  }

  m_nOverlapNum -= nNum;
  if (m_nOverlapNum <= 0)
  {
    memset_0(this, 0, sizeof(T));
  }
  return m_nOverlapNum;
}

template <typename T>
TInvenPage<T>::TInvenPage()
  : m_nMaxSlotNum(0),
    m_nMaxOverlapNum(0),
    m_pSlot(nullptr)
{
}

template <typename T>
TInvenPage<T>::~TInvenPage()
{
  if (m_pSlot)
  {
    delete[] m_pSlot;
    m_pSlot = nullptr;
  }
}

template <typename T>
bool TInvenPage<T>::create(int nMaxSlotNum, int nMaxOverlapNum)
{
  m_nMaxSlotNum = nMaxSlotNum;
  m_nMaxOverlapNum = nMaxOverlapNum;
  m_pSlot = new (std::nothrow) TInvenSlot<T>[static_cast<size_t>(m_nMaxSlotNum)];
  return m_pSlot != nullptr;
}

template <typename T>
TInvenSlot<T> *TInvenPage<T>::get_slot(int n)
{
  return m_pSlot ? &m_pSlot[n] : nullptr;
}

template <typename T>
int TInvenPage<T>::find_pos_empty()
{
  for (int j = 0; j < m_nMaxSlotNum; ++j)
  {
    if (m_pSlot[j].is_empty())
    {
      return j;
    }
  }
  return -1;
}

template <typename T>
int TInvenPage<T>::find_pos_overlap(const T *pItem, int nNum)
{
  for (int j = 0; j < m_nMaxSlotNum; ++j)
  {
    if (m_pSlot[j].able_overlap(pItem, nNum))
    {
      return j;
    }
  }
  return find_pos_empty();
}

template <typename T>
int TInvenPage<T>::find_empty(const T *pItem, int nNum)
{
  if (pItem && pItem->IsOverlapItem())
  {
    return find_pos_overlap(pItem, nNum);
  }
  return find_pos_empty();
}

template <typename T>
int TInvenPage<T>::push_overlap(const T *pItem, int nNum, int nS)
{
  if (nS < m_nMaxSlotNum)
  {
    return m_pSlot[nS].push(pItem, static_cast<unsigned int>(nNum));
  }
  return nNum;
}

template <typename T>
int TInvenPage<T>::push_normal(const T *pItem, int nS)
{
  if (nS < m_nMaxSlotNum)
  {
    return m_pSlot[nS].push(pItem, 1);
  }
  return 1;
}

template <typename T>
int TInvenPage<T>::push(const T *pItem, int nS, int nNum)
{
  if (!pItem)
  {
    return nNum;
  }
  if (pItem->IsOverlapItem())
  {
    return push_overlap(pItem, nNum, nS);
  }
  return push_normal(pItem, nS);
}

template <typename T>
int TInvenPage<T>::pop(const T *pItem, int nS, int nNum)
{
  if (nS < m_nMaxSlotNum)
  {
    return m_pSlot[nS].pop(pItem, nNum);
  }
  return -1;
}

template <typename T>
bool TInventory<T>::create(int nMaxPage, int nMaxSlot, int nMaxOverlap)
{
  m_nMaxPageNum = nMaxPage;
  m_nMaxSlotNum = nMaxSlot;
  m_nMaxOverlapNum = nMaxOverlap;
  m_pPage = new (std::nothrow) TInvenPage<T>[static_cast<size_t>(m_nMaxPageNum)];
  if (!m_pPage)
  {
    return false;
  }

  for (int j = 0; j < m_nMaxPageNum; ++j)
  {
    if (!m_pPage[j].create(m_nMaxSlotNum, m_nMaxOverlapNum))
    {
      return false;
    }
  }
  return true;
}

template <typename T>
TInvenSlot<T> *TInventory<T>::get_slot(int nP, int nS)
{
  if (m_nMaxPageNum > nP && m_nMaxSlotNum > nS)
  {
    return m_pPage[nP].get_slot(nS);
  }
  return nullptr;
}

template <typename T>
bool TInventory<T>::find_empty(const T *pItem, int nNum, int *nP, int *nS)
{
  *nS = -1;
  *nP = -1;
  *nS = m_pPage[m_nUsingPage].find_empty(pItem, nNum);
  if (*nS == -1)
  {
    while (*nS == -1)
    {
      if (++*nP >= m_nMaxPageNum)
      {
        break;
      }
      if (m_nUsingPage != *nP)
      {
        *nS = m_pPage[*nP].find_empty(pItem, nNum);
      }
    }
    if (*nS == -1)
    {
      *nP = -1;
      return false;
    }
    return true;
  }
  *nP = m_nUsingPage;
  return true;
}

template <typename T>
int TInventory<T>::push(int nP, int nS, const T *pItem, int nNum)
{
  if (nP < m_nMaxPageNum)
  {
    return m_pPage[nP].push(pItem, nS, nNum);
  }
  return nNum;
}

template <typename T>
int TInventory<T>::pop(int nP, int nS, const T *pItem, int nNum)
{
  if (pItem && m_nMaxPageNum > nP)
  {
    return m_pPage[nP].pop(pItem, nS, nNum);
  }
  return -1;
}

template <typename T>
void TInventory<T>::swap(TInvenSlot<T> *pSrc, TInvenSlot<T> *pDst)
{
  if (!pSrc || !pDst)
  {
    return;
  }

  T *pitem = pSrc->get_pitem();
  T *rhs = pDst->get_pitem();
  if (pitem && rhs && (*pitem == *rhs))
  {
    if (pitem->IsOverlapItem())
    {
      const unsigned int overlapnum = static_cast<unsigned int>(pSrc->get_overlapnum());
      const unsigned int dstOverlap = static_cast<unsigned int>(pDst->get_overlapnum());
      const int sum = static_cast<int>(overlapnum + dstOverlap);
      if (sum <= m_nMaxOverlapNum)
      {
        pDst->set_overlapnum(sum);
        pSrc->clear();
      }
      else
      {
        pDst->set_overlapnum(m_nMaxOverlapNum);
        pSrc->set_overlapnum(sum - m_nMaxOverlapNum);
      }
    }
  }
  else
  {
    TInvenSlot<T> temp(*pSrc);
    *pSrc = *pDst;
    *pDst = temp;
  }
}

#pragma pack(push, 1)
struct _pt_automine_charge_money_db_update_fail_zocl
{
  int nCharge;

  __int64 size();
};
#pragma pack(pop)

class AutoMineMachine
{
public:
  AutoMineMachine();
  ~AutoMineMachine();

  bool Initialize(unsigned __int8 byRace, unsigned __int8 byCollisionType);
  bool LoadDatabase(_DB_LOAD_AUTOMINE_MACHINE *pdata);
  bool SetOwner(unsigned __int8 byRace, unsigned __int8 byCollisionType, CGuild *pGuild);
  void GetMachineInfo(_DB_LOAD_AUTOMINE_MACHINE *pInfo);
  void Loop();
  void Start(int n);
  void Stop(int n);
  void SelectOre(int n, unsigned __int8 byOre);
  void Charge(int n, int nCharge, int nGold);
  void Discharge();
  void GetOutOreInAutoMine(CPlayer *pUser, char *pMsg);
  void MoveOreInAutoMine(int n, unsigned __int8 sl, unsigned __int8 ss, unsigned __int8 dl, unsigned __int8 ds);
  void OreMerge(int n, char *pMsg);
  void SendMsg_MachineInfo(unsigned int n);
  void SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode);
  void GetState(unsigned int *dwState);
  bool IsMaster(CPlayer *pUser);
  void SetOpenUI(bool bOpen);

  void SubChargeCost(unsigned __int8 byRet, char *pdata);
  void ChangeOwner(CGuild *pOwnerGuild);
  void push_dqs_newowner();
  void push_dqs_workingstate();
  void push_dqs_selore();
  void push_dqs_mineore(int nP, int nS);
  void push_dqs_getore(char nP, char nS, unsigned __int8 byNum);
  void push_dqs_moveore(
    unsigned __int8 nDBSlot_S,
    _INVENKEY *pSKey,
    unsigned __int8 bySNum,
    unsigned __int8 nDBSlot_D,
    _INVENKEY *pDKey,
    unsigned __int8 byDNum);
  void push_dqs_battery_charge(unsigned __int16 wIndex, int ncharge, int ncost);
  void push_dqs_battery_discharge();

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

private:
  bool _InitMineOre();
  void _Mining();
  unsigned int _Convert_GoldToGage(int nGold);
};

class AutoMineMachineMng
{
public:
  AutoMineMachineMng();
  ~AutoMineMachineMng() = default;

  static AutoMineMachineMng *Instance();
  static void Release();

  bool Initialzie();
  void Loop();
  bool OpenUI(int n);
  bool CloseUI(int n);
  bool StartWorkMachine(int n);
  bool StopWorkMachine(int n);
  bool SelectOreType(int n, char *pMsg);
  bool GetOutOre(int n, char *pMsg);
  bool MoveOrePos(int n, char *pMsg);
  bool BatteryCharge(int n, char *pMsg);
  bool OreMerge(int n, char *pMsg);
  bool Command(int n, int nType, char *pMsg);

  unsigned __int8 request_db_query(char *pdata);
  void result_db_query(unsigned __int8 byRet, char *pdata);
  AutoMineMachine *GetMachine(unsigned __int8 byRace, unsigned __int8 byCollisionType);
  void ChangeOwner(int nRaceCode, CGuild *pGuild, unsigned __int8 byCollisionType);

  void SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode);

private:
  unsigned __int8 get_type(CPlayer *pUser, unsigned __int8 byRace);

  unsigned __int8 _db_qry_insert_newowner(const _qry_case_amine_newowner *query);
  unsigned __int8 _db_qry_update_battery_charge(_qry_case_amine_batterycharge *query);
  unsigned __int8 _db_qry_update_mineore(const _qry_case_amine_mineore *query);
  unsigned __int8 _db_qry_update_workstate(const _qry_case_amine_workstate *query);
  unsigned __int8 _db_qry_update_selore(const _qry_case_amine_selore *query);
  unsigned __int8 _db_qry_update_battery_discharge(const _qry_case_amine_battery_discharge *query);
  unsigned __int8 _db_qry_update_moveore(const _qry_case_amine_moveore *query);

  AutoMineMachine m_Machine[3][2];

  static AutoMineMachineMng *m_pInstance;
};
