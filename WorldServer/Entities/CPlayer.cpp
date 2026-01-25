#include "pch.h"

#include "CPlayer.h"

_skill_fld *CPlayer::ms_pXmas_Snow_Effect = nullptr;
_skill_fld *CPlayer::ms_pXmas_Snow_Bullet_Effect = nullptr;

bool ItemCombineMgr::LoadData()
{
  return true;
}

CPlayer::CPlayer() = default;

void CPlayer::SetStaticMember()
{
}

void CPlayer::Init(_object_id *pID)
{
  (void)pID;
}

CPlayer::~CPlayer() = default;

void _WEAPON_PARAM::SetStaticMember(CRecordData *itemTable)
{
  (void)itemTable;
}

void _MASTERY_PARAM::SetStaticMember(CRecordData *effectTable, CRecordData *forceTable)
{
  (void)effectTable;
  (void)forceTable;
}
