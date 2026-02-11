#include "pch.h"

#include "CMainThread.h"
#include "CItemBox.h"
#include "CNetworkEX.h"
#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>

namespace
{
  const unsigned __int8 s_conver_index[2] = { 1, 0 };
}

void _personal_amine_mineore_zocl::clear()
{
  for (unsigned __int8 index = 0; index < byChangedNum; ++index)
  {
    change[index].wItemIndex = static_cast<unsigned __int16>(-1);
    change[index].wItemSerial = static_cast<unsigned __int16>(-1);
    change[index].dwDur = 0;
  }
  dwObjSerial = static_cast<unsigned int>(-1);
  dwBattery = 0;
  byChangedNum = 0;
}

unsigned int _personal_amine_mineore_zocl::size() const
{
  return 330U - 8U * (40U - byChangedNum);
}

_personal_automine_uninstall_zocl::_personal_automine_uninstall_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_uninstall_zocl::size() const
{
  return 24U - 6U * (2U - static_cast<unsigned int>(byCnt));
}

_personal_automine_uninstall_circle_zocl::_personal_automine_uninstall_circle_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_uninstall_circle_zocl::size() const
{
  return 5;
}

_personal_automine_current_state_zocl::_personal_automine_current_state_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_current_state_zocl::size() const
{
  return 7;
}

_personal_automine_install_zocl::_personal_automine_install_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_install_zocl::size() const
{
  return 27;
}

_personal_automine_battery_insert_zocl::_personal_automine_battery_insert_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_battery_insert_zocl::size() const
{
  return 10;
}

_personal_automine_battery_extract_zocl::_personal_automine_battery_extract_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_battery_extract_zocl::size() const
{
  return 11;
}

_personal_automine_selore_zocl::_personal_automine_selore_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_selore_zocl::size() const
{
  return 5;
}

_personal_automine_popore_zocl::_personal_automine_popore_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_popore_zocl::size() const
{
  return 3;
}

_personal_automine_stop_zocl::_personal_automine_stop_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_automine_stop_zocl::size() const
{
  return 11;
}

_personal_amine_infoui_open_zocl::_personal_amine_infoui_open_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

unsigned int _personal_amine_infoui_open_zocl::size() const
{
  return 12;
}

AP_BatterySlot::AP_BatterySlot()
  : m_bFill(false)
{
  std::memset(&battery_, 0, sizeof(battery_));
}

void AP_BatterySlot::clear()
{
  if (m_bFill)
  {
    m_bFill = false;
  }
  battery_.m_dwDur = 0;
}

bool AP_BatterySlot::extract(_STORAGE_LIST::_db_con *pout_item)
{
  if (!m_bFill)
  {
    return false;
  }

  memcpy_0(pout_item, &battery_, sizeof(_STORAGE_LIST::_db_con));
  m_bFill = false;
  battery_.m_bLoad = 0;
  battery_.m_dwDur = 0;
  return true;
}

unsigned int AP_BatterySlot::get_dur()
{
  if (m_bFill)
  {
    return static_cast<unsigned int>(battery_.m_dwDur);
  }
  return 0;
}

__int64 AP_BatterySlot::insert(_STORAGE_LIST::_db_con *pItem)
{
  if (m_bFill)
  {
    return -1;
  }
  if (!is_private_item(pItem))
  {
    return -2;
  }
  if (!pItem->m_dwDur)
  {
    return -3;
  }

  m_bFill = true;
  battery_.m_bLoad = 1;
  memcpy_0(&battery_, pItem, sizeof(battery_));
  return 0;
}

bool AP_BatterySlot::is_private_item(_STORAGE_LIST::_db_con *pItem)
{
  _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[pItem->m_byTableCode], pItem->m_wItemIndex);
  if (!strcmp_0(record->m_strCode, "itttt04"))
  {
    return true;
  }
  if (!strcmp_0(record->m_strCode, "itttt05"))
  {
    return true;
  }
  if (!strcmp_0(record->m_strCode, "itcsa01"))
  {
    return true;
  }
  return strcmp_0(record->m_strCode, "itcsa02") == 0;
}

bool AutominePersonal::is_installed()
{
  return m_bInstalled;
}

unsigned int AutominePersonal::get_objserial()
{
  return m_dwObjSerial;
}

unsigned int AutominePersonal::get_ownerserial()
{
  if (m_pOwner != nullptr)
  {
    return m_pOwner->m_Param.GetCharSerial();
  }
  return static_cast<unsigned int>(-1);
}

unsigned __int16 AutominePersonal::get_itemserial()
{
  return m_wItemSerial;
}

_STORAGE_LIST::_db_con *AutominePersonal::get_item()
{
  return m_pItem;
}

CPlayer *AutominePersonal::get_owner()
{
  return m_pOwner;
}

CPlayer *AutominePersonal::GetOwner()
{
  return m_pOwner;
}

bool AutominePersonal::is_run()
{
  return m_bStart;
}

void AutominePersonal::set_selore(unsigned __int8 bySelOre)
{
  m_bySelOre = bySelOre;
}

void AutominePersonal::set_openUI_Inven(bool bFlag)
{
  m_bOpenUI_Inven = bFlag;
}

void AutominePersonal::set_openUI_battery(bool bFlag)
{
  m_bOpenUI_Battery = bFlag;
}

void AutominePersonal::send_changed_packet(unsigned int n)
{
  if (m_bChanged)
  {
    m_bChanged = false;
    unsigned __int8 pbyType[2] = {14, 55};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(m_changed_packet.size());
    g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&m_changed_packet), nLen);
    m_changed_packet.clear();
  }
}

void AutominePersonal::set_work(bool bWork)
{
  m_bStart = bWork;
}

void AutominePersonal::sub_filledslot()
{
  --m_byFilledSlotCnt;
}

AutominePersonal::AutominePersonal()
  : m_bDBLoad(false),
    m_bOpenUI_Inven(false),
    m_bOpenUI_Battery(false),
    m_bInstalled(false),
    m_bInvenFull(false),
    m_bStart(false),
    m_bySelOre(0),
    m_wItemSerial(0),
    m_byFilledSlotCnt(0),
    m_dwNextSendTime_CurState(0),
    m_dwDelaySec(0),
    m_dwDelay(0),
    m_dwNextMineTime(0),
    m_dwChangeSendTime(0),
    m_nMaxHP(0),
    m_pItem(nullptr),
    m_pOwner(nullptr),
    m_byUseBattery(0),
    m_pBatterySlot(nullptr),
    m_bChanged(false)
{
  std::memset(&m_changed_packet, 0, sizeof(m_changed_packet));
  std::memset(m_dwMineCount, 0, sizeof(m_dwMineCount));
  std::memset(&m_update_mineore_old, 0, sizeof(m_update_mineore_old));
  std::memset(&m_update_mineore_new, 0, sizeof(m_update_mineore_new));
}

AutominePersonal::~AutominePersonal()
{
  m_bStart = false;
  m_bOpenUI_Battery = false;
  m_bOpenUI_Inven = false;
  m_wItemSerial = static_cast<unsigned __int16>(-1);
  m_pOwner = nullptr;
  m_pItem = nullptr;

  delete[] m_pBatterySlot;
  m_pBatterySlot = nullptr;
}

bool AutominePersonal::initialize(unsigned __int16 wIndex)
{
  char dest[256]{};
  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\SystemLog\\Concession\\AminePersonal.log");
  m_logProcess.SetWriteLogFile(dest, 1, 0, 1, 1);

  const unsigned int now = GetKorLocalTime();
  sprintf_s(dest, sizeof(dest), "..\\ZoneServerLog\\SystemLog\\log_AminePersonal_%u.log", now);
  m_logSysErr.SetWriteLogFile(dest, 1, 0, 1, 1);

  m_pBatterySlot = new AP_BatterySlot[2];
  if (m_pBatterySlot == nullptr)
  {
    return false;
  }

  _object_id objId{};
  objId.m_byKind = 0;
  objId.m_byID = 11;
  objId.m_wIndex = wIndex;
  CGameObject::Init(&objId);
  return true;
}

bool AutominePersonal::extract_battery(unsigned __int8 bySlotIdx, _STORAGE_LIST::_db_con *pBattery)
{
  if (bySlotIdx < 2)
  {
    if (m_pBatterySlot[bySlotIdx].extract(pBattery))
    {
      const unsigned __int8 otherIndex = s_conver_index[bySlotIdx];
      if (m_pBatterySlot[otherIndex].get_dur())
      {
        m_byUseBattery = otherIndex;
      }
      else
      {
        m_byUseBattery = static_cast<unsigned __int8>(-1);
      }
      send_current_state();
      return true;
    }
    return false;
  }

  m_logSysErr.Write("extract_battery(%d, n)::excess of max solt index", bySlotIdx);
  return false;
}

void AutominePersonal::send_current_state()
{
  if (m_pOwner != nullptr && m_pOwner->m_bOper)
  {
    m_dwNextSendTime_CurState += 60000;
    _personal_automine_current_state_zocl packet{};
    packet.wItemSerial = m_wItemSerial;
    packet.byFilledSlotCnt = m_byFilledSlotCnt;
    packet.dwBatteryGage = get_battery();
    unsigned __int8 pbyType[2] = {14, 67};
    g_Network.m_pProcess[0]->LoadSendMsg(
      m_pOwner->m_id.wIndex,
      pbyType,
      reinterpret_cast<char *>(&packet),
      static_cast<unsigned __int16>(packet.size()));
  }
}

unsigned int AutominePersonal::get_battery()
{
  const unsigned int first = m_pBatterySlot[0].get_dur();
  const unsigned int second = m_pBatterySlot[1].get_dur();
  return first + second;
}

unsigned int AutominePersonal::get_battery(int n)
{
  return m_pBatterySlot[n].get_dur();
}

bool AutominePersonal::unregist_from_map(unsigned __int8 byDestroyType)
{
  if (!m_bInstalled)
  {
    return false;
  }

  _personal_automine_uninstall_zocl uninstallPacket{};
  uninstallPacket.dwObjSerial = m_dwObjSerial;
  uninstallPacket.byActType = byDestroyType;
  uninstallPacket.dwOwnerSerial = get_ownerserial();
  uninstallPacket.wItemSerial = m_wItemSerial;

  if (byDestroyType && byDestroyType != 2)
  {
    const char *charName = m_pOwner->m_Param.GetCharNameA();
    m_logProcess.Write("AutominePersonal::unregist_from_map(destroy) >> Name:%s", charName);
  }
  else
  {
    for (int slotIndex = 0; slotIndex < 2; ++slotIndex)
    {
      _STORAGE_LIST::_db_con battery{};
      if (extract_battery(static_cast<unsigned __int8>(slotIndex), &battery))
      {
        if (m_pOwner->m_Param.m_pStoragePtr[0]->GetIndexEmptyCon() == 255)
        {
          CreateItemBox(
            &battery,
            m_pOwner,
            0xFFFFFFFF,
            false,
            m_pOwner,
            1u,
            m_pCurMap,
            m_wMapLayerIndex,
            m_fCurPos,
            false);
          CPlayer::s_MgrItemHistory.personal_amine_itemlog(
            "THROW_GROUND",
            0xFFu,
            battery.m_byTableCode,
            battery.m_wItemIndex,
            battery.m_dwDur,
            m_pOwner->m_szItemHistoryFileName);
        }
        else
        {
          _STORAGE_LIST::_db_con *added = m_pOwner->Emb_AddStorage(0, &battery, false, true);
          if (added)
          {
            uninstallPacket.battery[uninstallPacket.byCnt].dwDur = battery.m_dwDur;
            uninstallPacket.battery[uninstallPacket.byCnt++].wSerial = battery.m_wSerial;
            CPlayer::s_MgrItemHistory.personal_amine_itemlog(
              "MOVE_TO_INVEN",
              added->m_byStorageIndex,
              added->m_byTableCode,
              added->m_wItemIndex,
              added->m_dwDur,
              m_pOwner->m_szItemHistoryFileName);
          }
          else
          {
            const char *charName = m_pOwner->m_Param.GetCharNameA();
            m_logSysErr.Write(
              "AutominePersonal::unregist_from_map(recovery) >> Failed CPlayer::EmbAddStorage() >> Name:%s",
              charName);
          }
        }
      }
    }
  }

  unsigned __int8 sendType[2] = {14, 47};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pOwner->m_id.wIndex,
    sendType,
    reinterpret_cast<char *>(&uninstallPacket),
    static_cast<unsigned __int16>(uninstallPacket.size()));

  CPlayer::s_MgrItemHistory.personal_amine_uninstall(
    byDestroyType,
    m_dwMineCount,
    15,
    m_pItem,
    m_pOwner->m_szItemHistoryFileName);

  _personal_automine_uninstall_circle_zocl circlePacket{};
  circlePacket.dwObjSerial = m_dwObjSerial;
  circlePacket.byActType = byDestroyType;
  unsigned __int8 circleType[2] = {14, 65};
  CircleReport(circleType, reinterpret_cast<char *>(&circlePacket), static_cast<unsigned __int16>(circlePacket.size()), 0);

  m_pBatterySlot[0].clear();
  m_pBatterySlot[1].clear();
  m_changed_packet.clear();
  m_bDBLoad = false;
  m_bStart = false;
  m_pItem->m_bLock = 0;
  m_pOwner = nullptr;
  m_pItem = nullptr;
  m_bInstalled = false;
  m_wItemSerial = static_cast<unsigned __int16>(-1);
  m_byFilledSlotCnt = 0;
  return CGameObject::Destroy();
}

bool AutominePersonal::insert_battery(unsigned __int8 bySlotIdx, unsigned __int16 wItemSerial)
{
  if (bySlotIdx < 2u)
  {
    _STORAGE_LIST::_db_con *item =
      _STORAGE_LIST::GetPtrFromSerial(m_pOwner->m_Param.m_pStoragePtr[0], wItemSerial);
    if (!item)
    {
      m_logSysErr.Write("Is not exist battery.");
      return false;
    }

    if (m_pBatterySlot[bySlotIdx].insert(item))
    {
      return false;
    }

    m_byUseBattery = bySlotIdx;
    if (CPlayer::Emb_DelStorage(
          m_pOwner,
          0,
          item->m_byStorageIndex,
          0,
          1,
          "AutominePersonal::insert_battery()"))
    {
      if (!m_bStart)
      {
        m_dwNextMineTime = m_dwDelay + timeGetTime();
        m_bStart = true;
      }

      send_current_state();
      m_dwNextSendTime_CurState = GetLoopTime() + 60000;
      CPlayer::s_MgrItemHistory.personal_amine_itemlog(
        "BATTERY_IN",
        bySlotIdx,
        item->m_byTableCode,
        item->m_wItemIndex,
        item->m_dwDur,
        m_pOwner->m_szItemHistoryFileName);
      return true;
    }

    m_logSysErr.Write("insert_battery() - Emb_DelStorage Fail");
    m_pBatterySlot[bySlotIdx].clear();
    return false;
  }

  m_logSysErr.Write("insert_battery(%d, n)::excess of max solt index", bySlotIdx);
  return false;
}

bool AutominePersonal::regist_to_map(
  CPlayer *pOne,
  _STORAGE_LIST::_db_con *pDstItem,
  unsigned __int8 byDummyIndex,
  unsigned int dwObjSerial,
  float fDelayProf)
{
  if (m_bInstalled)
  {
    return false;
  }

  if (byDummyIndex >= pOne->m_pCurMap->m_nResDumNum)
  {
    return false;
  }

  _res_dummy *resDummy = &pOne->m_pCurMap->m_pResDummy[byDummyIndex];
  const int resDummySector = CMapData::GetResDummySector(pOne->m_pCurMap, byDummyIndex, pOne->m_fCurPos);
  if (resDummySector == -1)
  {
    return false;
  }

  if (resDummy == nullptr || _res_dummy::GetQualityGrade(resDummy) == 2)
  {
    return false;
  }

  const bool isPcBangType = _BILLING_INFO::IsPcBangType(&pOne->m_pUserDB->m_BillingInfo);
  m_dwDelay = static_cast<int>(
    static_cast<double>(
      _res_dummy::GetDelay(&pOne->m_pCurMap->m_pResDummy[byDummyIndex], resDummySector, isPcBangType))
    * 1.2);

  const double playerPenalty =
    TimeLimitMgr::GetPlayerPenalty(g_Main.m_pTimeLimitMgr, pOne->m_id.wIndex);
  int penaltyFactor = 0;
  if (playerPenalty != 0.0)
  {
    penaltyFactor = static_cast<int>(1.0 / playerPenalty);
  }

  if (fDelayProf > 1.0f)
  {
    m_dwDelay = static_cast<int>(static_cast<float>(static_cast<int>(m_dwDelay)) / fDelayProf);
  }

  m_dwDelaySec = static_cast<unsigned __int8>(m_dwDelay / 1000);
  m_dwDelay *= penaltyFactor;

  _object_create_setdata createData{};
  _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblItemData[pDstItem->m_byTableCode], pDstItem->m_wItemIndex);
  if (record == nullptr)
  {
    return false;
  }

  const int playerLevel = CPlayerDB::GetLevel(&pOne->m_Param);
  if (playerLevel < *reinterpret_cast<unsigned int *>(&record[4].m_strCode[52]))
  {
    return false;
  }

  createData.m_pRecordSet = record;
  createData.m_pMap = pOne->m_pCurMap;
  createData.m_nLayerIndex = pOne->m_wMapLayerIndex;
  CMapData::GetRandPosInRange(pOne->m_pCurMap, pOne->m_fCurPos, 10, createData.m_fStartPos);

  if (CGameObject::Create(&createData))
  {
    pDstItem->m_bLock = 1;
    m_bMove = false;
    m_dwObjSerial = dwObjSerial;
    m_pItem = pDstItem;
    m_pOwner = pOne;
    m_dwNextMineTime = static_cast<unsigned int>(-1);
    m_dwChangeSendTime = timeGetTime() + m_dwDelay + 3000;
    m_nMaxHP = *reinterpret_cast<int *>(&record[5].m_strCode[44]);
    m_wItemSerial = pDstItem->m_wSerial;
    m_byFilledSlotCnt = _STORAGE_LIST::GetNumUseCon(&pOne->m_Param.m_dbPersonalAmineInven);
    send_installed();
    m_bInstalled = true;
    std::memset(m_dwMineCount, 0, sizeof(m_dwMineCount));
    CPlayer::s_MgrItemHistory.personal_amine_install(
      m_pItem->m_byTableCode,
      m_pItem->m_wItemIndex,
      &m_pOwner->m_Param.m_dbPersonalAmineInven,
      m_pOwner->m_szItemHistoryFileName);
    return true;
  }

  m_logSysErr.Write("regist_to_map()::Failed CGameObject::Create(...)");
  return false;
}

void AutominePersonal::send_installed()
{
  _personal_automine_install_zocl packet{};
  packet.dwObjSerial = m_dwObjSerial;
  packet.wObjIndex = m_ObjID.m_wIndex;
  packet.dwOwnerSerial = get_ownerserial();
  packet.wItemSerial = m_pItem->m_wSerial;
  packet.wItemTblIndex = m_pItem->m_wItemIndex;
  memcpy_0(packet.fPos, m_fCurPos, sizeof(packet.fPos));
  packet.byFilledSlotCnt = m_byFilledSlotCnt;

  unsigned __int8 pbyType[2] = {14, 45};
  const unsigned int passObjSerial = get_ownerserial();
  const unsigned __int16 nLen = static_cast<unsigned __int16>(packet.size());
  CircleReport(pbyType, reinterpret_cast<char *>(&packet), nLen, passObjSerial, false);
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pOwner->m_id.wIndex,
    pbyType,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(packet.size()));
}

void AutominePersonal::set_delay(unsigned int dwDelay)
{
  m_dwDelay = dwDelay;
}

void AutominePersonal::set_delaysec(unsigned __int8 dwDS)
{
  m_dwDelaySec = dwDS;
}
