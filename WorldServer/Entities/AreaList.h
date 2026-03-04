#pragma once

#include <vector>
#include <string>
#include "AreaData.h"

struct AreaList
{
    std::vector<AreaData> List;
    unsigned int Count;
    unsigned int Width;
    unsigned int Height;
    char *pData;
    unsigned int DataEnd;
    char *pRealData;

    AreaList();
    ~AreaList();
    AreaList& operator=(const AreaList& rhs);
    void ExtractData();
    void Push(AreaData *adata);
};
