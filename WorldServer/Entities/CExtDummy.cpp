#include "pch.h"
#include "CExtDummy.h"

bool CExtDummy::LoadExtDummy(const char *szFileName)
{
    // Auth logic for loading EXT dummy file
    return true;
}

bool CExtDummy::GetWorldFromLocal(float *fOutPos, unsigned int nDummyIndex, float *v7)
{
    if (!fOutPos || !v7) return false;
    
    // Simplified: Just copy the input position as world position for now
    fOutPos[0] = v7[0];
    fOutPos[1] = v7[1];
    fOutPos[2] = v7[2];
    
    return true;
}
