#include "pch.h"
#include "CDummyPosTable.h"

bool CDummyPosTable::LoadDummyPosition(const char *szFileName, const char *szLabel)
{
    // Auth logic for loading SPT dummy file
    return true;
}

int CDummyPosTable::GetRecordNum()
{
    return m_nDumPosDataNum;
}

void *CDummyPosTable::GetRecord(int index)
{
    if (index < 0 || index >= m_nDumPosDataNum) return nullptr;
    return &m_pDumPos[index];
}

void *CDummyPosTable::GetRecord(const char *szLabel)
{
    for (int i = 0; i < m_nDumPosDataNum; ++i)
    {
        if (strcmp(m_pDumPos[i].m_szCode, szLabel) == 0)
            return &m_pDumPos[i];
    }
    return nullptr;
}
