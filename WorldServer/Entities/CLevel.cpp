#include "pch.h"
#include "CLevel.h"
#include "CBsp.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"

float CLevel::GetFirstYpos(float *fCenterPos, float *fMin, float *fMax)
{
    if (this->mIsLoadedBsp && this->mBsp)
        return this->mBsp->GetFirstYpos(fCenterPos, fMin, fMax);
    return 0.0f;
}

bool CLevel::LoadLevel(const char *szFileName)
{
    char Buffer[256];
    char szExtPath[256];
    char szRoot[256];

    if (!szFileName)
    {
        if (!this->mMapName[0]) return false;
        sprintf_s(Buffer, "%s\\%s\\%s.bsp", stState, this->mMapName, this->mMapName);
        szFileName = Buffer;
    }

    FILE* fp = nullptr;
    if (fopen_s(&fp, szFileName, "rb") == 0 && fp)
    {
        fclose(fp);
        strcpy_s(szExtPath, szFileName);
        StripEXT(szExtPath);
        StripPath(szExtPath);

        this->SetEnvironment(this, dword_184A79820);
        CBsp::LoadBsp(this->mBsp, (char*)szFileName);

        sprintf_s(szRoot, "%sEXT.spt", szExtPath);
        this->mDummy.LoadExtDummy(szRoot);
        this->mBsp->mDummy = &this->mDummy;

        this->mIsLoadedBsp = 1;
        this->mBsp->mIsLoaded = 1;
        return true;
    }
    
    if (this->mBsp) this->mBsp->mIsLoaded = 0;
    return false;
}

void CLevel::SetEnvironment(CLevel *pLevel, int nEnv)
{
    pLevel->mEnvironment = nEnv;
}
