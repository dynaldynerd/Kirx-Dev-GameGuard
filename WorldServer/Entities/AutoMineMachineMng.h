#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"
#include "CLogFile.h"

#include <new>

class CGuild;
class CPlayer;
class CMapData;

struct _AUTOMINE_BATTERY
{
  int m_nCurGage;
  int m_nMaxGage;
};

#pragma pack(push, 1)
struct __cppobj _AUTOMINE_SLOT
{
  int nLumpIndex;
  _INVENKEY item;
  int nOverlapNum;

  _AUTOMINE_SLOT();
};

struct __cppobj _DB_LOAD_AUTOMINE_MACHINE
{
  unsigned __int8 byCollisionType;
  unsigned __int8 byRace;
  unsigned int dwGuildSerial;
  unsigned int dwGMasterSerial;
  unsigned __int8 bWorking;
  unsigned __int8 bySelectedOre;
  unsigned int dwBatteryGage;
  unsigned __int8 bySlotCnt;
  _AUTOMINE_SLOT slot[80];

  _DB_LOAD_AUTOMINE_MACHINE();
  __int64 size() const;
};

struct __cppobj _pt_automine_info_zocl
{
  _DB_LOAD_AUTOMINE_MACHINE INFO;

  _pt_automine_info_zocl();
  int size();
};

struct __cppobj _pt_automine_result_zocl
{
  unsigned __int8 byType;
  unsigned __int8 byRetCode;

  __int64 size() const;
};

struct __cppobj _pt_automine_state_zocl
{
  unsigned __int8 bySelectOre;
  unsigned int dwGage;
  unsigned __int8 byPage;
  _INVENKEY item;

  _pt_automine_state_zocl();
  __int64 size() const;
};

struct __cppobj _pt_automine_getoutore_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int8 byRetCode;

  _pt_automine_getoutore_zocl();
  __int64 size() const;
};
#pragma pack(pop)

struct _pt_automine_charge_money_db_update_fail_zocl
{
  int nCharge;

  __int64 size();
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
  bool is_empty();
  bool able_overlap(T *pItem, int nNum);
  TInvenSlot *get_pitem();
  int get_overlapnum();
  void set_overlapnum(int n);
  int push(T *pItem, unsigned int nNum);
  int pop(T *pItem, int nNum);
};

template <typename T>
struct TInvenPage
{
  int m_nMaxSlotNum;
  int m_nMaxOverlapNum;
  TInvenSlot<T> *m_pSlot;

  TInvenPage();
  ~TInvenPage() = default;

  bool create(int nMaxSlotNum, int nMaxOverlapNum);
  void clear();
  TInvenSlot<T> *get_slot(int n);
  int find_pos_empty();
  int find_pos_overlap(T *pItem, int nNum);
  int find_empty(T *pItem, int nNum);
  int push(T *pItem, int nS, int nNum);
  int push_overlap(T *pItem, int nNum, int nS);
  int push_normal(T *pItem, int nS);
  int pop(T *pItem, int nS, int nNum);
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
  void clear();
  TInvenSlot<T> *get_slot(int nP, int nS);
  int push(int nP, int nS, T *pItem, int nNum);
  int pop(int nP, int nS, T *pItem, int nNum);
  bool find_empty(T *pItem, int nNum, int *nP, int *nS);
  void swap(TInvenSlot<T> *pSrc, TInvenSlot<T> *pDst);
};

template <typename T>
TInvenSlot<T>::TInvenSlot()
{
  new (&m_Item) T();
  m_nOverlapNum = 0;
  m_nMaxOverlapNum = 99;
  memset_0(this, 0, 4uLL);
}

template <typename T>
TInvenSlot<T>::TInvenSlot(const TInvenSlot &rhs)
{
  new (&m_Item) T();
  m_Item = rhs.m_Item;
  m_nOverlapNum = rhs.m_nOverlapNum;
  m_nMaxOverlapNum = rhs.m_nMaxOverlapNum;
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
void TInvenSlot<T>::clear()
{
  memset_0(this, 0, 4uLL);
  m_nOverlapNum = 0;
}

template <typename T>
bool TInvenSlot<T>::is_empty()
{
  return m_nOverlapNum == 0;
}

template <typename T>
bool TInvenSlot<T>::able_overlap(T *pItem, int nNum)
{
  return m_Item == *pItem && m_nMaxOverlapNum >= nNum + m_nOverlapNum;
}

template <typename T>
TInvenSlot<T> *TInvenSlot<T>::get_pitem()
{
  if (m_nOverlapNum)
  {
    return this;
  }
  return nullptr;
}

template <typename T>
int TInvenSlot<T>::get_overlapnum()
{
  return m_nOverlapNum;
}

template <typename T>
void TInvenSlot<T>::set_overlapnum(int n)
{
  m_nOverlapNum = n;
}

template <typename T>
int TInvenSlot<T>::push(T *pItem, unsigned int nNum)
{
  if (static_cast<int>(nNum) > m_nMaxOverlapNum)
  {
    return static_cast<int>(nNum);
  }

  if (m_Item == *pItem)
  {
    if (m_Item.IsOverlapItem())
    {
      const int sum = static_cast<int>(nNum) + m_nOverlapNum;
      int newOverlap = m_nOverlapNum;
      if (sum <= m_nMaxOverlapNum)
      {
        newOverlap = sum;
      }
      m_nOverlapNum = newOverlap;
      return sum - m_nOverlapNum;
    }
    return 1;
  }

  memcpy_0(this, pItem, 4uLL);
  m_nOverlapNum = static_cast<int>(nNum);
  return 0;
}

template <typename T>
int TInvenSlot<T>::pop(T *pItem, int nNum)
{
  if (!(m_Item == *pItem))
  {
    return -1;
  }

  m_nOverlapNum -= nNum;
  if (m_nOverlapNum <= 0)
  {
    memset_0(this, 0, 4uLL);
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
bool TInvenPage<T>::create(int nMaxSlotNum, int nMaxOverlapNum)
{
  m_nMaxSlotNum = nMaxSlotNum;
  m_nMaxOverlapNum = nMaxOverlapNum;
  m_pSlot = new (std::nothrow) TInvenSlot<T>[static_cast<size_t>(m_nMaxSlotNum)];
  return m_pSlot != nullptr;
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
TInvenSlot<T> *TInvenPage<T>::get_slot(int n)
{
  if (m_pSlot)
  {
    return &m_pSlot[n];
  }
  return nullptr;
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
int TInvenPage<T>::find_pos_overlap(T *pItem, int nNum)
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
int TInvenPage<T>::find_empty(T *pItem, int nNum)
{
  if (pItem && pItem->IsOverlapItem())
  {
    return find_pos_overlap(pItem, nNum);
  }
  return find_pos_empty();
}

template <typename T>
int TInvenPage<T>::push(T *pItem, int nS, int nNum)
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
int TInvenPage<T>::push_overlap(T *pItem, int nNum, int nS)
{
  if (nS < m_nMaxSlotNum)
  {
    return m_pSlot[nS].push(pItem, static_cast<unsigned int>(nNum));
  }
  return nNum;
}

template <typename T>
int TInvenPage<T>::push_normal(T *pItem, int nS)
{
  if (nS < m_nMaxSlotNum)
  {
    return m_pSlot[nS].push(pItem, 1);
  }
  return 1;
}

template <typename T>
int TInvenPage<T>::pop(T *pItem, int nS, int nNum)
{
  if (nS < m_nMaxSlotNum)
  {
    return m_pSlot[nS].pop(pItem, nNum);
  }
  return -1;
}

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
void TInventory<T>::clear()
{
  for (int j = 0; j < m_nMaxPageNum; ++j)
  {
    m_pPage[j].clear();
  }
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
int TInventory<T>::push(int nP, int nS, T *pItem, int nNum)
{
  if (nP < m_nMaxPageNum)
  {
    return m_pPage[nP].push(pItem, nS, nNum);
  }
  return nNum;
}

template <typename T>
int TInventory<T>::pop(int nP, int nS, T *pItem, int nNum)
{
  if (pItem && m_nMaxPageNum > nP)
  {
    return m_pPage[nP].pop(pItem, nS, nNum);
  }
  return -1;
}

template <typename T>
bool TInventory<T>::find_empty(T *pItem, int nNum, int *nP, int *nS)
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
void TInventory<T>::swap(TInvenSlot<T> *pSrc, TInvenSlot<T> *pDst)
{
  T *pitem = pSrc ? pSrc->get_pitem() : nullptr;
  T *rhs = pDst ? pDst->get_pitem() : nullptr;

  if (pitem && rhs && (*pitem == *rhs))
  {
    if (pitem->IsOverlapItem())
    {
      const int overlapnum = pSrc->get_overlapnum();
      const int dstOverlap = pDst->get_overlapnum();
      const int total = dstOverlap + overlapnum;
      if (total <= m_nMaxOverlapNum)
      {
        pDst->set_overlapnum(total);
        pSrc->clear();
      }
      else
      {
        pDst->set_overlapnum(m_nMaxOverlapNum);
        pSrc->set_overlapnum(total - m_nMaxOverlapNum);
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

class AutoMineMachine
{
public:
  AutoMineMachine();
  ~AutoMineMachine();

  __int64 _Convert_GoldToGage(int nGold);
  bool _InitMineOre();
  void _Mining();

  bool Initialize(unsigned __int8 byRace, unsigned __int8 byCollisionType);
  bool LoadDatabase(_DB_LOAD_AUTOMINE_MACHINE *pdata);
  void Loop();

  void SubChargeCost(unsigned __int8 byRet, char *pdata);
  void ChangeOwner(CGuild *pOwnerGuild);
  bool SetOwner(unsigned __int8 byRace, unsigned __int8 byCollisionType, CGuild *pGuild);
  void SetOpenUI(bool bOpen);
  bool IsMaster(CPlayer *pUser);
  CGuild *GetOwnerGuild();
  void GetMachineInfo(_DB_LOAD_AUTOMINE_MACHINE *pInfo);
  void GetState(unsigned int *dwState);

  void Start(int n);
  void Stop(int n);
  void SelectOre(int n, unsigned __int8 byOre);
  void GetOutOreInAutoMine(CPlayer *pUser, char *pMsg);
  void MoveOreInAutoMine(int n, unsigned __int8 sl, unsigned __int8 ss, unsigned __int8 dl, unsigned __int8 ds);
  void Charge(int n, int nCharge, int nGold);
  void Discharge();
  void OreMerge(int n, char *pMsg);

  void SendMsg_MachineInfo(unsigned int n);
  void SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode);

  void push_dqs_newowner();
  void push_dqs_battery_charge(unsigned __int16 wIndex, int ncharge, int ncost);
  void push_dqs_battery_discharge();
  void push_dqs_mineore(int nP, int nS);
  void push_dqs_getore(char nP, char nS, unsigned __int8 byNum);
  void push_dqs_workingstate();
  void push_dqs_selore();
  void push_dqs_moveore(
    unsigned __int8 nDBSlot_S,
    _INVENKEY *pSKey,
    unsigned __int8 bySNum,
    unsigned __int8 nDBSlot_D,
    _INVENKEY *pDKey,
    unsigned __int8 byDNum);

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

  bool OpenUI(int n);
  bool CloseUI(int n);
  bool StartWorkMachine(int n);
  bool StopWorkMachine(int n);
  bool SelectOreType(int n, char *pMsg);
  bool GetOutOre(int n, char *pMsg);
  bool MoveOrePos(int n, char *pMsg);
  bool BatteryCharge(int n, char *pMsg);
  bool OreMerge(int n, char *pMsg);

  unsigned __int8 request_db_query(char *pdata);
  void result_db_query(unsigned __int8 byRet, char *pdata);
  AutoMineMachine *GetMachine(unsigned __int8 byRace, unsigned __int8 byCollisionType);
  void ChangeOwner(int nRaceCode, CGuild *pGuild, unsigned __int8 byCollisionType);

private:
  unsigned __int8 get_type(CPlayer *pUser, unsigned __int8 byRace);
  void SendMsg_ResultCode(unsigned int n, unsigned __int8 byType, unsigned __int8 byRetCode);

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

inline _AUTOMINE_SLOT::_AUTOMINE_SLOT()
{
  new (&item) _INVENKEY();
}

inline _DB_LOAD_AUTOMINE_MACHINE::_DB_LOAD_AUTOMINE_MACHINE()
{
  for (int j = 0; j < 80; ++j)
  {
    slot[j] = _AUTOMINE_SLOT();
  }
  memset_0(this, 0, sizeof(_DB_LOAD_AUTOMINE_MACHINE));
}

inline __int64 _DB_LOAD_AUTOMINE_MACHINE::size() const
{
  return 977 - 12LL * (80 - bySlotCnt);
}

inline _pt_automine_info_zocl::_pt_automine_info_zocl()
{
  INFO = _DB_LOAD_AUTOMINE_MACHINE();
}

inline int _pt_automine_info_zocl::size()
{
  return static_cast<int>(INFO.size());
}

inline __int64 _pt_automine_result_zocl::size() const
{
  return 2;
}

inline _pt_automine_state_zocl::_pt_automine_state_zocl()
{
  new (&item) _INVENKEY();
}

inline __int64 _pt_automine_state_zocl::size() const
{
  return 10;
}

inline _pt_automine_getoutore_zocl::_pt_automine_getoutore_zocl()
{
  memset_0(this, 0, sizeof(_pt_automine_getoutore_zocl));
}

inline __int64 _pt_automine_getoutore_zocl::size() const
{
  return 3;
}
