#include "pch.h"

#include "CMonsterSPGroupTable.h"

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
    auto RecordNum = pMonsterRecordData->GetRecordNum();
    m_dwRecordNum = RecordNum;
    auto __n = this->m_dwRecordNum;
    _monster_sp_group* __t = new _monster_sp_group[310];
    m_pRecordData = __t;
    for (int n = 0; n < m_dwRecordNum; ++n)
    {
        auto v15 = &this->m_pRecordData[n];
        _monster_fld*Record = (_monster_fld*)pMonsterRecordData->GetRecord(n);
        v15->m_dwIndex = n;
        strcpy_s(v15->m_strCode, sizeof(v15->m_strCode), Record->m_strCode);
        v15->m_wCount = 0;
        for (int j = 0; j < 0xF; ++j)
        {
            if (!strcmp(Record->m_strSPCode[j], "-1"))
            {
                v15->m_pSPData[j] = NULL;
            }
            else
            {
                auto v18 = (_monster_sp_fld*)pMonsterSPRecordData->GetRecord(Record->m_strSPCode[(unsigned __int64)j]);
                if (!v18)
                    return false;
                v15->m_pSPData[v15->m_wCount] = v18;
                if (v18->m_nSpecialAttType)
                {
                    if (v18->m_nSpecialAttType == 1)
                    {
                        auto v10 = pForceRecordData->GetRecord(v18->m_strSpecialAttCode);
                        v15->m_SPDataFld[v15->m_wCount].m_SkillFld = (_skill_fld*)v10;
                    }
                    else
                    {
                        if (v18->m_nSpecialAttType != 2)
                            return false;
                        auto v11 = pClassSkillRecordData->GetRecord(v18->m_strSpecialAttCode);
                        v15->m_SPDataFld[v15->m_wCount].m_SkillFld = (_skill_fld*)v11;
                    }
                }
                else
                {
                    auto v9 = pSkillRecordData->GetRecord(v18->m_strSpecialAttCode);
                    v15->m_SPDataFld[v15->m_wCount].m_SkillFld = (_skill_fld*)v9;
                }
                if (v15->m_SPDataFld[v15->m_wCount].m_SkillFld)
                    ++v15->m_wCount;
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
