#include "pch.h"
#include "AreaList.h"

AreaList::AreaList()
{
    Count = 0;
    Width = 0;
    Height = 0;
    pData = nullptr;
    DataEnd = 0;
    pRealData = nullptr;
}

AreaList::~AreaList()
{
    if (pData)
    {
        operator delete[](pData);
        pData = nullptr;
    }
    if (pRealData)
    {
        operator delete[](pRealData);
        pRealData = nullptr;
    }
}

void AreaList::Push(AreaData *adata)
{
    List.push_back(*adata);
    Count++;
}

void AreaList::ExtractData()
{
    if (!pRealData && pData)
    {
        pRealData = new char[Height * Width];
        int v4 = 0;
        for (unsigned int j = 0; j < DataEnd; j += 3)
        {
            unsigned short v6 = *reinterpret_cast<unsigned short *>(&pData[j]);
            char v7 = pData[j + 2];
            for (int k = 0; k < v6 + 1; ++k)
            {
                pRealData[v4++] = v7;
            }
        }
    }
}
