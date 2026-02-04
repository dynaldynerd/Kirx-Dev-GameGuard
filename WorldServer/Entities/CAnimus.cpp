#include "pch.h"

#include "CAnimus.h"
#include "animus_fld.h"
#include "CGameObject.h"

#include <cstring>
#include <mmsystem.h>

CRecordData CAnimus::s_tblParameter[8];
unsigned int CAnimus::MAX_EXP[8]{};
int CAnimus::s_nLiveNum = 0;

namespace
{
  _animus_fld *GetAnimusFldFromLv(int nAnimusClass, unsigned int dwLv)
  {
    CRecordData *table = &CAnimus::s_tblParameter[nAnimusClass];
    for (int n = 0; n < 65; ++n)
    {
      _animus_fld *record = (_animus_fld*)table->GetRecord(n);
      if (record == nullptr)
      {
        return nullptr;
      }
      if (record->m_nLevel == dwLv)
      {
        return reinterpret_cast<_animus_fld *>(record);
      }
    }
    const int recordNum = static_cast<int>(table->GetRecordNum());
    return reinterpret_cast<_animus_fld *>(table->GetRecord(recordNum - 1));
  }
}

bool CAnimus::SetStaticMember()
{
  static const char *kFiles[8] = {
    ".\\script\\PaimonCharacter.dat",
    ".\\script\\HecateCharacter.dat",
    ".\\script\\InannaCharacter.dat",
    ".\\script\\IsisCharacter.dat",
    ".\\script\\PaimonCharacter_S.dat",
    ".\\script\\HecateCharacter_S.dat",
    ".\\script\\InannaCharacter_S.dat",
    ".\\script\\IsisCharacter_S.dat",
  };

  char errMsg[136]{};
  for (int nAnimusClass = 0; nAnimusClass < 8; ++nAnimusClass)
  {
    if (!CAnimus::s_tblParameter[nAnimusClass].ReadRecord(kFiles[nAnimusClass], 0x1B8u, errMsg))
    {
      MyMessageBox("CAnimus::SetStaticMember", errMsg);
      return false;
    }

    _animus_fld *record = GetAnimusFldFromLv(nAnimusClass, 0x40u);
    if (record == nullptr)
    {
      MyMessageBox("CAnimus::SetStaticMember", "::GetAnimusFldFromLv(...) Fail!");
      return false;
    }

    CAnimus::MAX_EXP[nAnimusClass] = static_cast<unsigned int>(record->m_nForLvUpExp - 1);
  }
  return true;
}

void CAnimus::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_byClassCode = static_cast<unsigned __int8>(-1);
  m_nHP = 0;
  m_nFP = 0;
  m_dwExp = 0;
  m_pMaster = nullptr;
  m_dwMasterSerial = 0;
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
  std::memset(m_aszMasterName, 0, sizeof(m_aszMasterName));
  m_byRoleCode = static_cast<unsigned __int8>(-1);
  m_fMoveSpeed = 0.0f;
  m_byPosRaceTown = static_cast<unsigned __int8>(-1);
  m_pBeforeTownCheckMap = nullptr;
  m_fBeforeTownCheckPos[0] = 0.0f;
  m_fBeforeTownCheckPos[1] = 0.0f;
  m_dwStunTime = 0;
  m_dwBeAttackedTargetTime = 0;
  m_pNextTarget = nullptr;
  m_nMaxAttackPnt = 0;
  m_pRecord = nullptr;
  m_nMaxHP = 0;
  m_nMaxFP = 0;
  m_Mightiness = 0.0f;
  for (int j = 0; j < 5; ++j)
  {
    m_DefPart[j] = 0;
  }
  m_dwAIMode = 0;
  m_pTarget = nullptr;
}

bool CAnimus::Destroy()
{
  m_dwLastDestroyTime = timeGetTime();
  SendMsg_Destroy();
  m_pMaster = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_tmNextEatMasterFP = static_cast<unsigned int>(-1);
  --CAnimus::s_nLiveNum;
  return CCharacter::Destroy();
}

void CAnimus::SendMsg_Destroy()
{
  char szMsg[7]{};
  memcpy_0(szMsg, &m_ObjID.m_wIndex, sizeof(m_ObjID.m_wIndex));
  memcpy_0(szMsg + 2, &m_dwObjSerial, sizeof(m_dwObjSerial));
  szMsg[6] = (m_nHP <= 0);

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 3;
  pbyType[1] = 26;
  CircleReport(pbyType, szMsg, 7, false);
}
