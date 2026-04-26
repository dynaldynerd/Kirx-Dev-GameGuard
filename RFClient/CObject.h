#pragma once

#include <windows.h>

#define ID_INVALID ((DWORD)-1L)
#define MAX_NAME_LENGTH (32)

#define MAX_CHAR_TYPE (6)
#define CTI_PLAYER 0
#define CTI_MONSTER 1
#define CTI_NPC 2

class CObject
{
protected:
  BYTE m_byObjectTypeID;
  DWORD m_dwIndex;
  DWORD m_dwServerIndex;
  char m_pName[MAX_NAME_LENGTH];

public:
  CObject();
  virtual ~CObject();

  BOOL IsEmpty(void) const;
  void SetEmpty(void);
  virtual BOOL IsEqual(BYTE pi_byObjectTypeID, DWORD pi_dwIndex) const;
  BOOL IsEqualIndex(DWORD pi_dwIndex) const;

  virtual void SetIndex(DWORD pi_dwIndex);
  DWORD GetIndex(void) const;
  void SetServerIndex(DWORD pi_dwIndex);
  DWORD GetServerIndex(void) const;

  void SetObjectTypeID(BYTE pi_byTypeID);
  BYTE GetObjectTypeID(void) const;

  virtual void SetName(const char *pi_pName);
  char *GetName(void);
  const char *GetName(void) const;
};
