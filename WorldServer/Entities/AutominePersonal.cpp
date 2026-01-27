#include "pch.h"

#include "CMainThread.h"
#include "KorLocalTime.h"

#include <cstdio>
#include <cstring>

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
