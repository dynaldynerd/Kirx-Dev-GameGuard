#include "pch.h"

#include "RFEvent_ClassRefine.h"

#include "CAsyncLogger.h"
#include "WorldServerUtil.h"

#include <cstring>

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
