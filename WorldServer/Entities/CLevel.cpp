#include "pch.h"
#include "CLevel.h"
#include "CBsp.h"

float CLevel::GetFirstYpos(float *fCenterPos, float *fMin, float *fMax)
{
    if (this->mIsLoadedBsp && this->mBsp)
        return this->mBsp->GetFirstYpos(fCenterPos, fMin, fMax);
    return 0.0f;
}

bool CLevel::LoadLevel(const char *szFileName)
{
    this->mIsLoadedBsp = 1;
    return true;
}
