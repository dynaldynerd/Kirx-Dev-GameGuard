#include "pch.h"
#include "AreaList.h"

AreaList::AreaList()
{
    Count = 0;
    Width = 0;
    Height = 0;
    DataEnd = 0;
}

void AreaList::Push(AreaData *adata)
{
    List.push_back(*adata);
    Count++;
}

void AreaList::ExtractData()
{
    if (m_RealData.empty() && !m_Data.empty())
    {
        m_RealData.resize(Height * Width);
        int v4 = 0;
        for (unsigned int j = 0; j < DataEnd; j += 3)
        {
            unsigned short v6 = *(unsigned short *)&m_Data[j];
            char v7 = m_Data[j + 2];
            for (int k = 0; k < v6 + 1; ++k)
            {
                if ((size_t)v4 < m_RealData.size())
                    m_RealData[v4++] = v7;
            }
        }
    }
}
