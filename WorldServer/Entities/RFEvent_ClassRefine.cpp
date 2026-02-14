#include "pch.h"

#include "RFEvent_ClassRefine.h"

#include "CAsyncLogger.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <cstring>

bool _event_participant_classrefine::IsChanged()
{
  return bChange;
}

__int64 _event_participant_classrefine::size()
{
  return 20;
}

bool RFEvent_ClassRefine::Initialzie()
{
  if (!GetLastWriteFileTime(".\\Initialize\\WorldSystem.ini", &m_ftWrite))
  {
    return false;
  }

  ReadClassRefineEventInfo();
  memcpy_0(&_kEvent, &_kModifyEvent, sizeof(_kEvent));

  _pkParticipant = reinterpret_cast<_event_participant_classrefine *>(operator new[](0xC5D0uLL));
  if (_pkParticipant == nullptr)
  {
    return false;
  }
  memset_0(_pkParticipant, 0, 0xC5D0uLL);
  m_bUserDataReset = false;

  m_tmDataFileCheckTime.BeginTimer(0x2710u);
  CAsyncLogger *logger = CAsyncLogger::Instance();
  logger->Regist(
    ALT_CLASSREFINE_SERVICE_LOG,
    "..\\ZoneServerLog\\ServiceLog\\ClassRefine",
    "ClassRefine_EventLog",
    true,
    0xFFFFFFFF);

  const char *enableText = _kEvent.bEnable ? "TRUE" : "FALSE";
  CAsyncLogger *formatLogger = CAsyncLogger::Instance();
  formatLogger->FormatLog(
    9,
    "[EVENT_CLASSREFINE] : %s : StartDate=%d, EndDate=%d",
    enableText,
    _kEvent.nStartDate,
    _kEvent.nEndDate);

  return true;
}

void RFEvent_ClassRefine::Loop()
{
  if (m_tmDataFileCheckTime.CountingTimer() && CheckRefineEventData())
  {
    memcpy_0(&_kEvent, &_kModifyEvent, sizeof(_kEvent));
    if (m_bUserDataReset)
    {
      ResetRefineData();
    }
  }
}

bool RFEvent_ClassRefine::CheckRefineEventData()
{
  _FILETIME fileWriteTime{};
  if (!GetLastWriteFileTime(".\\Initialize\\WorldSystem.ini", &fileWriteTime))
  {
    return false;
  }

  if (m_ftWrite.dwHighDateTime == fileWriteTime.dwHighDateTime
      && m_ftWrite.dwLowDateTime == fileWriteTime.dwLowDateTime)
  {
    return false;
  }

  memcpy_s(&m_ftWrite, sizeof(m_ftWrite), &fileWriteTime, sizeof(fileWriteTime));
  ReadClassRefineEventInfo();

  if (_kEvent.bEnable == _kModifyEvent.bEnable && _kEvent.nStartDate == _kModifyEvent.nStartDate
      && _kEvent.nEndDate == _kModifyEvent.nEndDate && _kEvent.nMaxRefineCnt == _kModifyEvent.nMaxRefineCnt)
  {
    return false;
  }

  if ((!_kEvent.bEnable && _kModifyEvent.bEnable) || _kModifyEvent.nStartDate > _kEvent.nStartDate)
  {
    m_bUserDataReset = true;
  }

  return true;
}

void RFEvent_ClassRefine::ResetRefineData()
{
  for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
  {
    CPlayer *player = &g_Player[playerIndex];
    if (!player->m_bOper)
    {
      continue;
    }

    _pkParticipant[playerIndex].dwRefineDate = _kEvent.nStartDate;
    _pkParticipant[playerIndex].nCurRefineCnt = 0;
    _pkParticipant[playerIndex].bChange = true;
  }
}

unsigned __int8 RFEvent_ClassRefine::CanDoEvent(CPlayer *pOne)
{
  if (!_kEvent.bEnable)
  {
    return 6;
  }

  const unsigned int localDate = GetLocalDate();
  if (localDate < static_cast<unsigned int>(_kEvent.nStartDate)
      || localDate > static_cast<unsigned int>(_kEvent.nEndDate))
  {
    return 6;
  }

  if (!pOne->m_bLive || !pOne->m_bOper)
  {
    return 8;
  }

  const unsigned __int16 playerIndex = pOne->m_ObjID.m_wIndex;
  if (pOne->m_Param.GetCharSerial() == _pkParticipant[playerIndex].nAvatorSerial)
  {
    return 10;
  }

  if (_pkParticipant[playerIndex].nCurRefineCnt < _kEvent.nMaxRefineCnt)
  {
    return 0;
  }

  return 7;
}

unsigned __int8 RFEvent_ClassRefine::DoEvent(CPlayer *pOne)
{
  unsigned __int8 result = CanDoEvent(pOne);
  if (result)
  {
    return result;
  }

  result = pOne->pc_InitClass();
  if (result)
  {
    return result;
  }

  const unsigned __int16 playerIndex = pOne->m_ObjID.m_wIndex;
  ++_pkParticipant[playerIndex].nCurRefineCnt;
  _pkParticipant[playerIndex].bChange = true;
  return 0;
}

char RFEvent_ClassRefine::SetEvent(const char *p, int size, bool bInit)
{
  if (size != 16)
  {
    return 0;
  }

  memcpy_0(&_kEvent, p, size);
  if (bInit)
  {
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x53u, nullptr, 0);
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      _pkParticipant[index].bChange = false;
      _pkParticipant[index].dwRefineDate = static_cast<unsigned int>(_kEvent.nStartDate);
      _pkParticipant[index].nCurRefineCnt = 0;
    }
  }
  return 1;
}

bool RFEvent_ClassRefine::IsDbUpdate(unsigned int nIdx)
{
  return nIdx < 0x9E4 && _pkParticipant[nIdx].IsChanged();
}

_event_participant_classrefine *RFEvent_ClassRefine::GetPlayerState(unsigned int nIdx, unsigned int nAvator)
{
  if (nIdx >= 0x9E4)
  {
    return nullptr;
  }
  if (_pkParticipant[nIdx].nAvatorSerial == nAvator)
  {
    return &_pkParticipant[nIdx];
  }
  return nullptr;
}

bool RFEvent_ClassRefine::SetPlayerState(void *const p, int size)
{
  if (size != 20)
  {
    return false;
  }

  auto *state = reinterpret_cast<_event_participant_classrefine *>(p);
  const unsigned __int16 index = state->nSock;

  _pkParticipant[index].nAvatorSerial = state->nAvatorSerial;
  if (state->dwRefineDate >= _kEvent.nStartDate)
  {
    _pkParticipant[index].dwRefineDate = state->dwRefineDate;
    _pkParticipant[index].nCurRefineCnt = state->nCurRefineCnt;
    _pkParticipant[index].bChange = false;
  }
  else
  {
    _pkParticipant[index].dwRefineDate = _kEvent.nStartDate;
    _pkParticipant[index].nCurRefineCnt = 0;
    _pkParticipant[index].bChange = true;
  }
  _pkParticipant[index].nSock = index;

  return true;
}

void RFEvent_ClassRefine::ReadClassRefineEventInfo()
{
  char returnedString[24]{};
  memset(returnedString, 0, 8);
  GetPrivateProfileStringA(
    "CLASS REFINE EVENT",
    "ENABLE",
    "FALSE",
    returnedString,
    8u,
    ".\\Initialize\\WorldSystem.ini");

  const unsigned int localDate = GetLocalDate();
  _kModifyEvent.nStartDate = GetPrivateProfileIntA(
    "CLASS REFINE EVENT",
    "START_DATE",
    localDate - 1,
    ".\\Initialize\\WorldSystem.ini");

  const unsigned int localDateEnd = GetLocalDate();
  _kModifyEvent.nEndDate = GetPrivateProfileIntA(
    "CLASS REFINE EVENT",
    "END_DATE",
    localDateEnd - 1,
    ".\\Initialize\\WorldSystem.ini");

  _kModifyEvent.nMaxRefineCnt = GetPrivateProfileIntA(
    "CLASS REFINE EVENT",
    "LIMIT_COUNT",
    0,
    ".\\Initialize\\WorldSystem.ini");

  _kModifyEvent.bEnable = strcmp_0(returnedString, "TRUE") == 0;
}
