#include "pch.h"

#include "CMainThread.h"
#include "CItemBox.h"
#include "CNetworkEX.h"
#include "CUserDB.h"
#include "KorLocalTime.h"
#include "personal_amine_errmsg_zocl.h"
#include "personal_automine_install_zocl.h"

#include <cstdio>
#include <cstring>
#include <mmsystem.h>

namespace
{
  const unsigned __int8 s_conver_index[2] = { 1, 0 };

  unsigned int GetResDummyDelay(const _res_dummy *dummy, int sector, bool isPcBang)
  {
    const int safeSector = (sector >= 0 && sector < 3) ? sector : 2;
    return dummy->m_dwDelay[safeSector][isPcBang ? 1 : 0];
  }
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

bool AP_BatterySlot::is_private_item(_STORAGE_LIST::_db_con *pItem)
{
  if (pItem == nullptr)
  {
    return false;
  }

  _base_fld *record =
    reinterpret_cast<_base_fld *>(g_Main.m_tblItemData[pItem->m_byTableCode].GetRecord(pItem->m_wItemIndex));
  if (record == nullptr)
  {
    return false;
  }

  return strcmp_0(record->m_strCode, "itttt04") == 0
      || strcmp_0(record->m_strCode, "itttt05") == 0
      || strcmp_0(record->m_strCode, "itcsa01") == 0
      || strcmp_0(record->m_strCode, "itcsa02") == 0;
}

int AP_BatterySlot::insert(_STORAGE_LIST::_db_con *pItem)
{
  if (m_bFill)
  {
    return -1;
  }

  if (!is_private_item(pItem))
  {
    return -2;
  }

  if (pItem->m_dwDur == 0)
  {
    return -3;
  }

  m_bFill = true;
  battery_.m_bLoad = 1;
  memcpy_0(&battery_, pItem, sizeof(battery_));
  return 0;
}

bool AutominePersonal::is_installed()
{
  return m_bInstalled;
}

bool AutominePersonal::regist_to_map(
  CPlayer *pOwner,
  _STORAGE_LIST::_db_con *pInstallItem,
  unsigned __int8 byDummyIndex,
  unsigned int dwObjSerial,
  float fDelayProf)
{
  if (m_bInstalled)
  {
    return false;
  }

  if (pOwner == nullptr || pInstallItem == nullptr || pOwner->m_pCurMap == nullptr)
  {
    return false;
  }

  if (byDummyIndex >= pOwner->m_pCurMap->m_nResDumNum)
  {
    return false;
  }

  _res_dummy *resDummy = &pOwner->m_pCurMap->m_pResDummy[byDummyIndex];
  const int sector = pOwner->m_pCurMap->GetResDummySector(byDummyIndex, pOwner->m_fCurPos);
  if (sector == -1)
  {
    return false;
  }

  if (resDummy == nullptr || resDummy->GetQualityGrade() == 2)
  {
    return false;
  }

  const bool isPcBang = pOwner->m_pUserDB->m_BillingInfo.IsPcBangType();
  m_dwDelay = static_cast<unsigned int>(GetResDummyDelay(resDummy, sector, isPcBang) * 1.2f);

  const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(pOwner->m_id.wIndex);
  int penaltyScale = 0;
  if (playerPenalty != 0.0)
  {
    penaltyScale = static_cast<int>(1.0 / playerPenalty);
  }

  if (fDelayProf > 1.0f)
  {
    m_dwDelay = static_cast<unsigned int>(static_cast<float>(m_dwDelay) / fDelayProf);
  }

  m_dwDelaySec = static_cast<unsigned __int8>(m_dwDelay / 1000);
  m_dwDelay *= static_cast<unsigned int>(penaltyScale);

  _object_create_setdata createData{};
  _base_fld *itemRecord =
    reinterpret_cast<_base_fld *>(g_Main.m_tblItemData[pInstallItem->m_byTableCode].GetRecord(pInstallItem->m_wItemIndex));
  if (itemRecord == nullptr)
  {
    return false;
  }

  if (pOwner->m_Param.GetLevel() < *reinterpret_cast<int *>(&itemRecord[4].m_strCode[52]))
  {
    return false;
  }

  createData.m_pRecordSet = itemRecord;
  createData.m_pMap = pOwner->m_pCurMap;
  createData.m_nLayerIndex = pOwner->m_wMapLayerIndex;
  pOwner->m_pCurMap->GetRandPosInRange(pOwner->m_fCurPos, 10, createData.m_fStartPos);

  if (!CGameObject::Create(&createData))
  {
    m_logSysErr.Write("regist_to_map()::Failed CGameObject::Create(...)");
    return false;
  }

  pInstallItem->m_bLock = 1;
  m_bMove = false;
  m_dwObjSerial = dwObjSerial;
  m_pItem = pInstallItem;
  m_pOwner = pOwner;
  m_dwNextMineTime = static_cast<unsigned int>(-1);
  m_dwChangeSendTime = timeGetTime() + m_dwDelay + 3000;
  m_nMaxHP = *reinterpret_cast<int *>(&itemRecord[5].m_strCode[44]);
  m_wItemSerial = pInstallItem->m_wSerial;
  m_byFilledSlotCnt = static_cast<unsigned __int8>(m_pOwner->m_Param.m_dbPersonalAmineInven.GetNumUseCon());

  send_installed();

  m_bInstalled = true;
  memset_0(m_dwMineCount, 0, sizeof(m_dwMineCount));

  CPlayer::s_MgrItemHistory.personal_amine_itemlog(
    "INSTALL",
    m_pItem->m_byStorageIndex,
    m_pItem->m_byTableCode,
    m_pItem->m_wItemIndex,
    m_pItem->m_dwDur,
    m_pOwner->m_szItemHistoryFileName);
  return true;
}

unsigned int AutominePersonal::get_objserial()
{
  return m_dwObjSerial;
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

unsigned int AutominePersonal::get_ownerserial()
{
  if (m_pOwner != nullptr)
  {
    return m_pOwner->m_Param.GetCharSerial();
  }
  return static_cast<unsigned int>(-1);
}

CPlayer *AutominePersonal::GetOwner()
{
  return m_pOwner;
}

int AutominePersonal::insert_battery(unsigned __int8 bySlotIdx, unsigned __int16 wItemSerial)
{
  if (bySlotIdx >= 2)
  {
    m_logSysErr.Write("insert_battery(%d, n)::excess of max solt index", bySlotIdx);
    return 0;
  }

  _STORAGE_LIST::_db_con *item = m_pOwner->m_Param.m_pStoragePtr[0]->GetPtrFromSerial(wItemSerial);
  if (item == nullptr)
  {
    m_logSysErr.Write("Is not exist battery.");
    return 0;
  }

  if (m_pBatterySlot[bySlotIdx].insert(item) != 0)
  {
    return 0;
  }

  m_byUseBattery = bySlotIdx;
  if (!m_pOwner->Emb_DelStorage(0, item->m_byStorageIndex, 0, true, "AutominePersonal::insert_battery()"))
  {
    m_logSysErr.Write("insert_battery() - Emb_DelStorage Fail");
    m_pBatterySlot[bySlotIdx].clear();
    return 0;
  }

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
  return 1;
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

void AutominePersonal::LoadDBComplete()
{
  m_bDBLoad = true;
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

void AutominePersonal::set_selore(unsigned __int8 bySelOre)
{
  m_bySelOre = bySelOre;
}

void AutominePersonal::set_work(bool bWork)
{
  m_bStart = bWork;
}

bool AutominePersonal::is_run()
{
  return m_bStart;
}

void AutominePersonal::sub_filledslot()
{
  --m_byFilledSlotCnt;
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
  if (!m_bChanged)
  {
    return;
  }

  m_bChanged = false;
  unsigned __int8 type[2] = {14, 55};
  const unsigned __int16 packetSize = static_cast<unsigned __int16>(
    10u + 8u * static_cast<unsigned int>(m_changed_packet.byChangedNum));
  g_Network.m_pProcess[0]->LoadSendMsg(
    static_cast<int>(n),
    type,
    reinterpret_cast<char *>(&m_changed_packet),
    packetSize);
  m_changed_packet.clear();
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

  unsigned __int8 type[2] = {14, 45};
  const unsigned int ownerSerial = get_ownerserial();
  CircleReport(type, reinterpret_cast<char *>(&packet), static_cast<unsigned __int16>(packet.size()), ownerSerial, false);
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pOwner->m_id.wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(packet.size()));
}

void AutominePersonal::send_ecode(unsigned __int8 byCode)
{
  if (m_pOwner == nullptr || !m_pOwner->m_bOper)
  {
    return;
  }

  _personal_amine_errmsg_zocl msg{};
  msg.byErrCode = byCode;
  unsigned __int8 type[2] = {14, 62};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_pOwner->m_id.wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(msg.size()));
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
