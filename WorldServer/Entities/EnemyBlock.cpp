#include "pch.h"
#include "CMainThread.h"
#include "CMapData.h"
#include "CObjectList.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mmsystem.h>

bool _mon_block::SetBlock(_mon_block_fld *pBlkRec, CMapData *pMap, _dummy_position **ppDumPos)
{
    this->m_pBlkRec = pBlkRec;
    this->m_pMap = pMap;
    
    char Buffer[64];
    sprintf_s(Buffer, "%sbarea", pMap->m_pMapSet->m_strCode);
    size_t len = strlen(Buffer);
    
    this->m_bBossBlock = (strncmp(pBlkRec->m_strCode, Buffer, len) == 0);
    
    for (unsigned int j = 0; j < pBlkRec->m_dwDummyNum; ++j)
        this->m_pDumPos[j] = ppDumPos[j];
        
    return true;
}

void _mon_block::SetRotateBlock(bool IsRotateBlock)
{
    this->m_bRotate = IsRotateBlock;
}

int _mon_block::SelectDummyIndex()
{
    return rand() % this->m_pBlkRec->m_dwDummyNum;
}

bool _mon_active::SetActive(_mon_active_fld *pRec, _mon_block *pBlk, int nMonRecIndex)
{
    if (this->m_pBlk)
        return false;
        
    this->m_pActRec = pRec;
    this->m_pBlk = pBlk;
    this->m_wMonRecIndex = (unsigned short)nMonRecIndex;
    this->m_nLimRegenNum = pRec->m_dwRegenLimNum;
    
    return true;
}

void _mon_active::SetZeroMonNum()
{
    this->m_zCurMonNum = 0;
    this->m_dwCumMonNum = 0;
}

char _mon_active::SetCurMonNum(int nAlter)
{
    this->m_zCurMonNum = static_cast<signed __int16>(this->m_zCurMonNum + nAlter);
    if (nAlter > 0)
    {
        this->m_dwCumMonNum += static_cast<unsigned int>(nAlter);
    }
    BossScheduleSave();
    return 1;
}

void _mon_active::SetBossSchedule(BossSchedule *pBossSchedule)
{
    this->m_pBossSchedule = pBossSchedule;
}

BossSchedule *_mon_active::GetBossSchedule()
{
    return this->m_pBossSchedule;
}

void _mon_active::BossScheduleSave()
{
    BossSchedule *bossSchedule = GetBossSchedule();
    if (bossSchedule)
    {
        bossSchedule->Save_LiveCount(static_cast<unsigned __int16>(this->m_zCurMonNum));
    }
}

void _LAYER_SET::CreateLayer(int nSecNum)
{
    this->m_pMB = nullptr;
    this->m_nSecNum = nSecNum;
    this->m_pListSectorObj = new CObjectList[nSecNum];
    this->m_pListSectorPlayer = new CObjectList[nSecNum];
    this->m_pListSectorTower = new CObjectList[nSecNum];
}

void _LAYER_SET::ActiveLayer(_MULTI_BLOCK *pMB)
{
    this->m_pMB = pMB;
    for (int j = 0; j < this->m_pMB->m_nBlockNum; ++j)
    {
        int RecordNum = this->m_pMB->m_ptbMonBlock[j].m_Header.m_nRecordNum;
        for (int k = 0; k < RecordNum; ++k)
        {
            this->m_MonAct[j][k] = this->m_pMB->m_ppMonAct[j][k];
            this->m_MonAct[j][k].SetZeroMonNum();
        }
    }
    this->m_dwStartActiveTime = timeGetTime();
}

char _LAYER_SET::InertLayer()
{
    this->m_pMB = nullptr;
    return 1;
}

bool _LAYER_SET::IsActiveLayer()
{
    return this->m_pMB != nullptr;
}
