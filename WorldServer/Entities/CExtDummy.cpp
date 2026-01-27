#include "pch.h"
#include "CExtDummy.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

CExtDummy::CExtDummy()
{
    this->mDummy = nullptr;
    this->mNum = 0;
    this->mMaxNum = 0;
}

CExtDummy::~CExtDummy()
{
    if (this->mDummy)
        delete[] this->mDummy;
}

bool CExtDummy::LoadExtDummy(const char *szFileName)
{
    FILE *Stream = nullptr;
    if (fopen_s(&Stream, szFileName, "rt") != 0 || !Stream)
    {
        this->mNum = 0;
        return false;
    }

    this->mNum = 0;
    this->mMaxNum = 16;
    this->mDummy = new _EXT_DUMMY[this->mMaxNum];

    char String[256];
    bool bScriptBegin = false;
    while (fscanf_s(Stream, "%s", String, (unsigned int)sizeof(String)) != -1)
    {
        if (strcmp(String, "script_begin") == 0)
        {
            bScriptBegin = true;
            break;
        }
    }

    if (!bScriptBegin)
    {
        fclose(Stream);
        return false;
    }

    while (fscanf_s(Stream, "%s", String, (unsigned int)sizeof(String)) != -1)
    {
        _strlwr_s(String, strlen(String) + 1);
        if (strcmp(String, "script_end") == 0) break;

        if (String[0] == '*')
        {
            if (this->mNum >= this->mMaxNum)
            {
                unsigned int newMax = this->mMaxNum + 32;
                _EXT_DUMMY *newDummy = new _EXT_DUMMY[newMax];
                memcpy(newDummy, this->mDummy, sizeof(_EXT_DUMMY) * this->mMaxNum);
                delete[] this->mDummy;
                this->mDummy = newDummy;
                this->mMaxNum = newMax;
            }

            _EXT_DUMMY &item = this->mDummy[this->mNum];
            memset(&item, 0, sizeof(_EXT_DUMMY));
            strcpy_s(item.mName, String);

            // Mocking identity matrix initialization
            for(int i=0; i<4; ++i) for(int j=0; j<4; ++j) item.mMat[i][j] = (i==j) ? 1.0f : 0.0f;

            for (int i = 0; i < 3; ++i) fscanf_s(Stream, "%f", &item.mBBmin[i]);
            for (int i = 0; i < 3; ++i) fscanf_s(Stream, "%f", &item.mBBmax[i]);

            while (fscanf_s(Stream, "%s", String, (unsigned int)sizeof(String)) != -1)
            {
                if (String[0] == '*')
                {
                   // fseek(Stream, -(long)strlen(String), SEEK_CUR); // This is tricky with fscanf
                   break;
                }
                if (strcmp(String, "script_end") == 0) break;
                
                if (strcmp(String, "-node_tm") == 0)
                {
                    for (int i = 0; i < 4; ++i)
                        for (int j = 0; j < 4; ++j)
                            fscanf_s(Stream, "%f", &item.mMat[i][j]);
                }
                else if (strcmp(String, "-id") == 0)
                {
                    fscanf_s(Stream, "%d", &item.mID);
                }
                else if (strcmp(String, "-music") == 0) item.mFlag |= 1;
                else if (strcmp(String, "-interior") == 0) item.mFlag |= 4;
                // Add more flags if needed from IDA
            }
            this->mNum++;
        }
    }

    fclose(Stream);
    return true;
}

bool CExtDummy::GetWorldFromLocal(float *fOutPos, unsigned int nDummyIndex, float *v7)
{
    if (!mDummy || nDummyIndex >= mNum) return false;
    
    _EXT_DUMMY &item = mDummy[nDummyIndex];
    
    // Vector3fTransform logic (Simplified Matrix * Vector)
    float x = v7[0], y = v7[1], z = v7[2];
    fOutPos[0] = x * item.mMat[0][0] + y * item.mMat[1][0] + z * item.mMat[2][0] + item.mMat[3][0];
    fOutPos[1] = x * item.mMat[0][1] + y * item.mMat[1][1] + z * item.mMat[2][1] + item.mMat[3][1];
    fOutPos[2] = x * item.mMat[0][2] + y * item.mMat[1][2] + z * item.mMat[2][2] + item.mMat[3][2];
    
    return true;
}
