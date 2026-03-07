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

AreaList& AreaList::operator=(const AreaList& rhs)
{
    List = rhs.List;
    Count = rhs.Count;
    Width = rhs.Width;
    Height = rhs.Height;
    pData = rhs.pData;
    DataEnd = rhs.DataEnd;
    pRealData = rhs.pRealData;
    return *this;
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
        int outputIndex = 0;
        for (unsigned int j = 0; j < DataEnd; j += 3)
        {
            unsigned short repeatCount = *reinterpret_cast<unsigned short *>(&pData[j]);
            char areaValue = pData[j + 2];
            for (int k = 0; k < repeatCount + 1; ++k)
            {
                pRealData[outputIndex++] = areaValue;
            }
        }
    }
}
