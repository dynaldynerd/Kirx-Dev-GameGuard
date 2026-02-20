#include "pch.h"
#include "CMainThread.h"
#include "CDummyPosTable.h"
#include "CGameObject.h"
#include "WorldServerUtil.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <string>

CDummyPosTable::CDummyPosTable()
{
    this->m_pDumPos = nullptr;
    this->m_nDumPosDataNum = 0;
}

_dummy_position *CDummyPosTable::ms_pHeroes_Dummy[3];
const char *C_HEROES_DUMMY_NAME[3] = {"sd030f1", "sd040f1", "sd050f1"};

CDummyPosTable::~CDummyPosTable()
{
    if (this->m_pDumPos)
        delete[] this->m_pDumPos;
}

bool CDummyPosTable::LoadDummyPosition(const char *szFileName, const char *szLabel)
{
    FILE *stream = nullptr;
    if (fopen_s(&stream, szFileName, "rt") != 0 || !stream)
        return false;

    char token[132]{};
    const size_t prefixLength = strlen(szLabel);

    int matchedCount = 0;
    while (fscanf_s(stream, "%s", token, static_cast<unsigned int>(sizeof(token))) != EOF)
    {
        if (strncmp(token, szLabel, prefixLength) == 0)
        {
            ++matchedCount;
        }
        else if (strcmp(token, "[HelperObjectEnd]") == 0)
        {
            break;
        }
    }

    this->m_nDumPosDataNum = matchedCount;
    this->m_pDumPos = new _dummy_position[matchedCount];

    rewind(stream);

    int dummyIndex = 0;
    int helperLineIndex = 0;
    while (fscanf_s(stream, "%s", token, static_cast<unsigned int>(sizeof(token))) != EOF)
    {
        if (token[0] == '*')
        {
            if (strncmp(token, szLabel, prefixLength) == 0)
            {
                _dummy_position &dummy = this->m_pDumPos[dummyIndex];
                dummy.m_wLineIndex = static_cast<unsigned short>(helperLineIndex);

                std::string loweredCode = token + 1; // Keep behavior: stored code excludes '*' prefix.
                std::transform(
                    loweredCode.begin(),
                    loweredCode.end(),
                    loweredCode.begin(),
                    [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
                strcpy_s(dummy.m_szCode, loweredCode.c_str());

                for (int axis = 0; axis < 3; ++axis)
                {
                    int value = 0;
                    fscanf_s(stream, "%d", &value);
                    dummy.m_zLocalMin[axis] = static_cast<short>(value);
                }

                for (int axis = 0; axis < 3; ++axis)
                {
                    int value = 0;
                    fscanf_s(stream, "%d", &value);
                    dummy.m_zLocalMax[axis] = static_cast<short>(value);
                }

                ++dummyIndex;
            }

            ++helperLineIndex;
        }
        else if (strcmp(token, "[HelperObjectEnd]") == 0)
        {
            break;
        }
    }

    fclose(stream);
    return true;
}

bool CDummyPosTable::FindDummy(char *pszTextFileName, char *pszDummyCode, _dummy_position *pDummyPos)
{
    if (!pszTextFileName || !pszDummyCode || !pDummyPos)
        return false;

    FILE *Stream = nullptr;
    if (fopen_s(&Stream, pszTextFileName, "rt") != 0 || !Stream)
        return false;

    char destination[128]{};
    strcpy_s(destination, "*");
    strcat_s(destination, pszDummyCode);

    char token[256];
    int foundCount = 0;
    while (fscanf_s(Stream, "%s", token, (unsigned int)sizeof(token)) != -1)
    {
        if (strcmp(token, destination) == 0)
        {
            ++foundCount;
        }
        else if (strcmp(token, "[HelperObjectEnd]") == 0)
        {
            break;
        }
    }

    if (foundCount != 1)
    {
        fclose(Stream);
        return false;
    }

    rewind(Stream);
    int lineIndex = 0;
    while (fscanf_s(Stream, "%s", token, (unsigned int)sizeof(token)) != -1)
    {
        if (token[0] == '*')
        {
            if (strcmp(token, destination) == 0)
            {
                pDummyPos->m_wLineIndex = static_cast<unsigned short>(lineIndex);
                _strlwr_s(token, sizeof(token));
                strcpy_s(pDummyPos->m_szCode, token);
                for (int j = 0; j < 3; ++j)
                {
                    int value = 0;
                    fscanf_s(Stream, "%d", &value);
                    pDummyPos->m_zLocalMin[j] = static_cast<short>(value);
                }
                for (int j = 0; j < 3; ++j)
                {
                    int value = 0;
                    fscanf_s(Stream, "%d", &value);
                    pDummyPos->m_zLocalMax[j] = static_cast<short>(value);
                }
                break;
            }
            ++lineIndex;
        }
        else if (strcmp(token, "[HelperObjectEnd]") == 0)
        {
            break;
        }
    }

    fclose(Stream);
    return true;
}

bool CDummyPosTable::CheckHeroesDummy(CGameObject *pObj, unsigned __int8 byRaceCode)
{
    if (!pObj)
        return false;
    if (byRaceCode >= 3)
        return false;
    if (!CDummyPosTable::ms_pHeroes_Dummy[byRaceCode])
        return true;
    if (pObj->IsInTown())
        return GetSqrt(CDummyPosTable::ms_pHeroes_Dummy[byRaceCode]->m_fCenterPos, pObj->m_fCurPos) <= 100.0f;
    return false;
}

int CDummyPosTable::GetRecordNum()
{
    return m_nDumPosDataNum;
}

void *CDummyPosTable::GetRecord(int index)
{
    if (index < m_nDumPosDataNum)
        return &m_pDumPos[index];
    return nullptr;
}

void *CDummyPosTable::GetRecord(const char *szLabel)
{
    char destination[72]{};
    char *duplicatedCode = _strdup(szLabel);
    char *lowerCode = _strlwr(duplicatedCode);
    strcpy_s(destination, lowerCode);
    free(duplicatedCode);

    for (int i = 0; i < m_nDumPosDataNum; ++i)
    {
        if (strcmp(m_pDumPos[i].m_szCode, destination) == 0)
            return &m_pDumPos[i];
    }
    return nullptr;
}
