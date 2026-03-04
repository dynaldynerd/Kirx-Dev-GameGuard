#include "pch.h"

#include "CMainThread.h"

RFEventBase::RFEventBase()
{
}

bool RFEventBase::Initialzie()
{
  return false; // this is not a stub
}

bool RFEventBase::IsEnable()
{
  return false; // this is not a stub
}

bool RFEventBase::SetEvent(const char *p, int size, bool bInit)
{
  (void)p;
  (void)size;
  (void)bInit;
  return false; // this is not a stub
}

void RFEventBase::Loop()
{
  // this is not a stub
}

unsigned __int8 RFEventBase::DoEvent(CPlayer * /*pOne*/)
{
  // this is not a stub
  return 0;
}

bool RFEventBase::IsDbUpdate(unsigned int /*nIdx*/)
{
  // this is not a stub
  return false;
}

_event_participant_classrefine *RFEventBase::GetPlayerState(unsigned int /*nIdx*/, unsigned int /*nAvator*/)
{
  // this is not a stub
  return nullptr;
}

bool RFEventBase::SetPlayerState(void *const /*p*/, int /*size*/)
{
  // this is not a stub
  return false;
}
