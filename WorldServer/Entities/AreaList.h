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
    std::vector<char> m_Data;
    unsigned int DataEnd;
    std::vector<char> m_RealData;

    AreaList();
    void ExtractData();
    void Push(AreaData *adata);
};
