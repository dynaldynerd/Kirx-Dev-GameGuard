#include "pch.h"
#include "CDummyPosTable.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

CDummyPosTable::CDummyPosTable()
{
    this->m_pDumPos = nullptr;
    this->m_nDumPosDataNum = 0;
}

CDummyPosTable::~CDummyPosTable()
{
    if (this->m_pDumPos)
        delete[] this->m_pDumPos;
}

bool CDummyPosTable::LoadDummyPosition(const char *szFileName, const char *szLabel)
{
    FILE *Stream = nullptr;
    if (fopen_s(&Stream, szFileName, "rt") != 0 || !Stream)
        return false;

    char Str1[256];
    int count = 0;
    size_t prefixLen = strlen(szLabel);

    while (fscanf_s(Stream, "%s", Str1, (unsigned int)sizeof(Str1)) != -1)
    {
        if (strncmp(Str1, szLabel, prefixLen) == 0)
        {
            count++;
        }
        else if (strcmp(Str1, "[HelperObjectEnd]") == 0)
        {
            break;
        }
    }

    this->m_nDumPosDataNum = count;
    this->m_pDumPos = new _dummy_position[count];

    rewind(Stream);
    int idx = 0;
    int lineIdx = 0;
    while (fscanf_s(Stream, "%s", Str1, (unsigned int)sizeof(Str1)) != -1)
    {
        if (Str1[0] == '*')
        {
            if (strncmp(Str1, szLabel, prefixLen) == 0)
            {
                this->m_pDumPos[idx].m_wLineIndex = (unsigned short)lineIdx;
                char* pCode = Str1;
                _strlwr_s(pCode, strlen(pCode) + 1);
                strcpy_s(this->m_pDumPos[idx].m_szCode, pCode);

                for (int j = 0; j < 3; ++j)
                    fscanf_s(Stream, "%d", &this->m_pDumPos[idx].m_zLocalMin[j]);
                for (int j = 0; j < 3; ++j)
                    fscanf_s(Stream, "%d", &this->m_pDumPos[idx].m_zLocalMax[j]);
                idx++;
            }
            lineIdx++;
        }
        else if (strcmp(Str1, "[HelperObjectEnd]") == 0)
        {
            break;
        }
    }

    fclose(Stream);
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
