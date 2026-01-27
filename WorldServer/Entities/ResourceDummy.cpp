#include "pch.h"
#include "CMainThread.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

_res_dummy::_res_dummy()
{
    memset(this, 0, sizeof(*this));
    this->m_pDumPos = nullptr;
}

bool _res_dummy::SetDummy(_dummy_position *pDumPos, unsigned char byQualityGrade)
{
    if (this->m_pDumPos)
        return false;

    this->m_pDumPos = pDumPos;
    this->m_byQualityGrade = byQualityGrade;

    if (!this->m_byQualityGrade)
    {
        this->m_dwDelay[0][0] = 10000;
        this->m_dwDelay[0][1] = 16000;
        this->m_dwDelay[1][0] = 18000;
        this->m_dwDelay[1][1] = 24000;
        this->m_dwDelay[2][0] = 26000;
        this->m_dwDelay[2][1] = 30000;

        float dx = (float)(this->m_pDumPos->m_zLocalMax[0] - this->m_pDumPos->m_zLocalMin[0]);
        float dz = (float)(this->m_pDumPos->m_zLocalMax[2] - this->m_pDumPos->m_zLocalMin[2]);

        float v10 = sqrtf((2.0f * (dx * dx)) / 3.0f);
        float v12 = sqrtf((2.0f * (dz * dz)) / 3.0f);
        float v13 = sqrtf((dx * dx) / 3.0f);
        float v15 = sqrtf((dz * dz) / 3.0f);

        for (int j = 0; j < 3; ++j)
        {
            this->m_fMinLocal[2][j] = (float)this->m_pDumPos->m_zLocalMin[j];
            this->m_fMaxLocal[2][j] = (float)this->m_pDumPos->m_zLocalMax[j];
        }

        this->m_fMinLocal[1][0] = this->m_fMinLocal[2][0] + (float)((dx - v10) / 2.0);
        this->m_fMinLocal[1][1] = this->m_fMinLocal[2][1];
        this->m_fMinLocal[1][2] = this->m_fMinLocal[2][2] + (float)((dz - v12) / 2.0);
        this->m_fMaxLocal[1][0] = this->m_fMaxLocal[2][0] - (float)((dx - v10) / 2.0);
        this->m_fMaxLocal[1][1] = this->m_fMaxLocal[2][1];
        this->m_fMaxLocal[1][2] = this->m_fMaxLocal[2][2] - (float)((dz - v12) / 2.0);

        this->m_fMinLocal[0][0] = this->m_fMinLocal[2][0] + (float)((dx - v13) / 2.0);
        this->m_fMinLocal[0][1] = this->m_fMinLocal[2][1];
        this->m_fMinLocal[0][2] = this->m_fMinLocal[2][2] + (float)((dz - v15) / 2.0);
        this->m_fMaxLocal[0][0] = this->m_fMaxLocal[2][0] - (float)((dx - v13) / 2.0);
        this->m_fMaxLocal[0][1] = this->m_fMaxLocal[2][1];
        this->m_fMaxLocal[0][2] = this->m_fMaxLocal[2][2] - (float)((dz - v15) / 2.0);

        for (int i = 0; i < 10; ++i)
            this->SetRangeGrade();
    }
    return true;
}

void _res_dummy::SetRangeGrade()
{
    if (!this->m_byQualityGrade)
    {
        unsigned char v5 = rand() % 3;
        int v3 = rand();
        char v6 = (char)((v3 % 2) + 1); // Simplified from (v3 >> 31) ^ v3 & 1
        
        this->m_byGrade[v5++] = 0;
        this->m_byGrade[v5++ % 3] = v6;
        this->m_byGrade[v5 % 3] = (v6 == 1) ? 2 : 1;
    }
}
