#include "CObject.h"

#include <cstring>

CObject::CObject()
  : m_byObjectTypeID(static_cast<BYTE>(ID_INVALID)),
    m_dwIndex(ID_INVALID),
    m_dwServerIndex(ID_INVALID)
{
  ZeroMemory(m_pName, sizeof(m_pName));
}

CObject::~CObject()
{
}

BOOL CObject::IsEmpty(void) const
{
  return m_dwIndex == ID_INVALID;
}

void CObject::SetEmpty(void)
{
  m_dwIndex = ID_INVALID;
}

BOOL CObject::IsEqual(BYTE pi_byObjectTypeID, DWORD pi_dwIndex) const
{
  return (m_byObjectTypeID == pi_byObjectTypeID) && (m_dwIndex == pi_dwIndex);
}

BOOL CObject::IsEqualIndex(DWORD pi_dwIndex) const
{
  return m_dwIndex == pi_dwIndex;
}

void CObject::SetIndex(DWORD pi_dwIndex)
{
  m_dwIndex = pi_dwIndex;
}

DWORD CObject::GetIndex(void) const
{
  return m_dwIndex;
}

void CObject::SetServerIndex(DWORD pi_dwIndex)
{
  m_dwServerIndex = pi_dwIndex;
}

DWORD CObject::GetServerIndex(void) const
{
  return m_dwServerIndex;
}

void CObject::SetObjectTypeID(BYTE pi_byTypeID)
{
  m_byObjectTypeID = pi_byTypeID;
}

BYTE CObject::GetObjectTypeID(void) const
{
  return m_byObjectTypeID;
}

void CObject::SetName(const char *pi_pName)
{
  if (!pi_pName)
  {
    m_pName[0] = '\0';
    return;
  }

  strncpy_s(m_pName, sizeof(m_pName), pi_pName, _TRUNCATE);
}

char *CObject::GetName(void)
{
  return m_pName;
}

const char *CObject::GetName(void) const
{
  return m_pName;
}
