#include "pch.h"

#include "CMainThread.h"
#include "CItemBox.h"
#include "CNetworkEX.h"
#include "KorLocalTime.h"

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

bool AutominePersonal::is_installed()
{
  return m_bInstalled;
}

unsigned int AutominePersonal::get_ownerserial()
{
  if (m_pOwner != nullptr)
  {
    return CPlayerDB::GetCharSerial(&m_pOwner->m_Param);
  }
  return static_cast<unsigned int>(-1);
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
    const char *charName = CPlayerDB::GetCharNameA(&m_pOwner->m_Param);
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
            const char *charName = CPlayerDB::GetCharNameA(&m_pOwner->m_Param);
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
