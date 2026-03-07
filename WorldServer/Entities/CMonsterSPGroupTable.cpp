#include "pch.h"

#include "CMainThread.h"
#include "CMonsterSPGroupTable.h"

#include <cstring>

CMonsterSPGroupTable::CMonsterSPGroupTable()
  : m_dwRecordNum(0),
    m_pRecordData(nullptr)
{
}

CMonsterSPGroupTable::~CMonsterSPGroupTable()
{
  if (m_pRecordData)
  {
    operator delete[](m_pRecordData);
    m_pRecordData = nullptr;
  }
}

bool CMonsterSPGroupTable::Create(
  CRecordData * pMonsterRecordData,
  CRecordData * pMonsterSPRecordData,
  CRecordData * pSkillRecordData,
  CRecordData * pForceRecordData,
  CRecordData * pClassSkillRecordData)
{
    if (!pMonsterSPRecordData || !pMonsterRecordData || !pSkillRecordData || !pForceRecordData || !pClassSkillRecordData)
        return false;
    if (pMonsterSPRecordData->GetRecordNum() <= 0)
        return false;
    const int recordNum = pMonsterRecordData->GetRecordNum();
    m_dwRecordNum = recordNum;
    const size_t recordCount = static_cast<size_t>(m_dwRecordNum);
    _monster_sp_group* groups = new (std::nothrow) _monster_sp_group[recordCount];
    if (!groups)
        return false;
    std::memset(groups, 0, sizeof(_monster_sp_group) * recordCount);
    m_pRecordData = groups;
    for (int n = 0; n < m_dwRecordNum; ++n)
    {
        _monster_sp_group *group = &this->m_pRecordData[n];
        _monster_fld *monsterRecord = (_monster_fld*)pMonsterRecordData->GetRecord(n);
        group->m_dwIndex = n;
        strcpy_s(group->m_strCode, sizeof(group->m_strCode), monsterRecord->m_strCode);
        group->m_wCount = 0;
        for (int j = 0; j < 0xF; ++j)
        {
            if (!strcmp(monsterRecord->m_strSPCode[j], "-1"))
            {
                group->m_pSPData[j] = NULL;
            }
            else
            {
                _monster_sp_fld *monsterSPRecord =
                  (_monster_sp_fld*)pMonsterSPRecordData->GetRecord(monsterRecord->m_strSPCode[(unsigned __int64)j]);
                if (!monsterSPRecord)
                    return false;
                group->m_pSPData[group->m_wCount] = monsterSPRecord;
                if (monsterSPRecord->m_nSpecialAttType)
                {
                    if (monsterSPRecord->m_nSpecialAttType == 1)
                    {
                        auto *forceSkillRecord = (_skill_fld*)pForceRecordData->GetRecord(monsterSPRecord->m_strSpecialAttCode);
                        group->m_SPDataFld[group->m_wCount].m_SkillFld = forceSkillRecord;
                    }
                    else
                    {
                        if (monsterSPRecord->m_nSpecialAttType != 2)
                            return false;
                        auto *classSkillRecord =
                          (_skill_fld*)pClassSkillRecordData->GetRecord(monsterSPRecord->m_strSpecialAttCode);
                        group->m_SPDataFld[group->m_wCount].m_SkillFld = classSkillRecord;
                    }
                }
                else
                {
                    auto *skillRecord = (_skill_fld*)pSkillRecordData->GetRecord(monsterSPRecord->m_strSpecialAttCode);
                    group->m_SPDataFld[group->m_wCount].m_SkillFld = skillRecord;
                }
                if (group->m_SPDataFld[group->m_wCount].m_SkillFld)
                    ++group->m_wCount;
            }
        }
    }
    return true;
}

_monster_sp_group *CMonsterSPGroupTable::GetRecord(unsigned int dwIndex)
{
    if (dwIndex >= m_dwRecordNum)
        return nullptr;
    return &m_pRecordData[dwIndex];
}

_monster_sp_group *CMonsterSPGroupTable::GetRecord(char *szCode)
{
    for (unsigned int j = 0; j < m_dwRecordNum; ++j)
    {
        if (!strcmp_0(m_pRecordData[j].m_strCode, szCode))
            return &m_pRecordData[j];
    }
    return nullptr;
}
