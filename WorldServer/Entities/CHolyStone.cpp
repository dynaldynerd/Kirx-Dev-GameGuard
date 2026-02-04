#include "pch.h"
#include "CHolyStone.h"

int CHolyStone::s_nLiveNum = 0;
unsigned int CHolyStone::s_dwSerialCnt = 0;

bool CHolyStone::Create(_stone_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
    return false;

  m_pRec = (_monster_fld *)m_pRecordSet;
  m_bOper = false;
  m_nHP = static_cast<int>(m_pRec->m_fMaxHP);
  m_nOldRate = 10000;
  m_dwLastRecoverTime = GetLoopTime();
  m_pRec->m_fHPRecDelay = 3.0f;
  m_pRec->m_fHPRecUnit = 1000.0f;
  m_nMaxHP = static_cast<int>(m_pRec->m_fMaxHP);
  m_pDum = pData->pDumPosition;
  m_byMasterRaceCode = pData->byMasterRace;
  for (int j = 0; j < 5; ++j)
    m_nDefPart[j] = static_cast<int>(m_pRec->m_fStdDefFc);
  m_dwObjSerial = CHolyStone::GetNewStoneSerial();
  SendMsg_Create();
  ++CHolyStone::s_nLiveNum;
  return true;
}

unsigned int CHolyStone::GetNewStoneSerial()
{
  return s_dwSerialCnt++;
}

void CHolyStone::SendMsg_Create()
{
  struct
  {
    unsigned __int16 recordIndex;
    unsigned __int16 objIndex;
    unsigned int objSerial;
    __int16 pos[3];
    unsigned __int8 masterRace;
  } msg{};

  msg.recordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.objIndex = m_ObjID.m_wIndex;
  msg.objSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.pos, 3);
  msg.masterRace = m_byMasterRaceCode;

  unsigned __int8 pbyType[28]{};
  pbyType[0] = 3;
  pbyType[1] = static_cast<unsigned __int8>(-45);
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 15, false);
}

void CHolyStone::SetOper(bool bOper, float fHPRate)
{
  if (m_bOper != bOper)
  {
    m_bOper = bOper;
    if (m_bOper)
    {
      m_nHP = static_cast<int>(m_pRec->m_fMaxHP * fHPRate);
      if (m_nHP <= 0)
        m_nHP = 1;
      m_nMaxHP = m_nHP;
    }
    SendMsg_StoneAlterOper();
  }
}

void CHolyStone::SendMsg_StoneAlterOper()
{
  struct
  {
    unsigned int objSerial;
    unsigned __int8 isOper;
    unsigned __int16 hpRate;
  } msg{};

  msg.objSerial = m_dwObjSerial;
  msg.isOper = m_bOper ? 1 : 0;
  msg.hpRate = static_cast<unsigned __int16>(CalcCurHPRate());

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 25;
  pbyType[1] = 14;
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 7, false);
}

unsigned int CHolyStone::CalcCurHPRate()
{
  return static_cast<unsigned int>(static_cast<int>((static_cast<float>(m_nHP) / static_cast<float>(m_nMaxHP)) * 10000.0f));
}

unsigned int CHolyStone::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

char CHolyStone::SetHP(int nHP, bool bOver)
{
  int targetHP = nHP;
  if (targetHP < 0)
    targetHP = 0;
  if (!bOver && targetHP > m_nMaxHP)
    targetHP = m_nMaxHP;
  if (m_nHP == targetHP)
    return 0;
  m_nHP = targetHP;
  return 1;
}

bool CHolyStone::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_bOper = false;
  m_nHP = 0;
  m_nMaxHP = 0;
  m_pRec = nullptr;
  m_pDum = nullptr;
  m_byMasterRaceCode = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 5; ++j)
    m_nDefPart[j] = 0;
  m_nOldRate = 0;
  m_dwLastRecoverTime = 0;
  m_nCurrLootIndex = -1;
  m_nEndLootIndex = -1;
  m_nCurrDropIndex = -1;
  m_wMagnifications = 0;
  m_wRange = 0;
  m_wDropCntOnce = 0;
  m_wAddCountWithPlayer = 0;
  return true;
}
