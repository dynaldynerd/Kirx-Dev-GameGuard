#include "pch.h"

#include "CMapData.h"

#include <cstring>
#include "CExtDummy.h"
#include "CLevel.h"
#include "MonsterSetInfoData.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

void CMapData::Init(_map_fld *pMapSet)
{
  this->m_nMapCode = pMapSet->m_dwIndex;
  this->m_pMapSet = pMapSet;
}

bool CMapData::OpenMap(char *pszMapCode, _map_fld *pMapSet, int nType)
{
  if (pMapSet->m_nLayerNum > 500)
  {
    MyMessageBox("CMapData Error", "%s: layer num (%d > %d)..", pszMapCode, pMapSet->m_nLayerNum, 500);
    return false;
  }

  if (!pMapSet->m_nMapType)
  {
    if (pMapSet->m_nLayerNum > 1) pMapSet->m_nLayerNum = 1;
    if (pMapSet->m_nMonsterSetFileNum > 1) pMapSet->m_nMonsterSetFileNum = 1;
  }

  this->m_ls = new _LAYER_SET[pMapSet->m_nLayerNum];
  this->m_mb = new _MULTI_BLOCK[pMapSet->m_nMonsterSetFileNum];
  this->m_pMapSet = pMapSet;
  this->m_bUse = (nType != 0);
  this->m_nMapIndex = pMapSet->m_dwIndex;

  char Buffer[256];
  sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
  if (!this->m_Dummy.LoadExtDummy(Buffer))
  {
      MyMessageBox("CMapData Error", "read dummy-list fail(%s)", pszMapCode);
      return false;
  }

  sprintf_s(Buffer, ".\\map\\%s\\%sEXT.spt", pszMapCode, pszMapCode);
  // Implementation for Town Dummy simplified for parity with LoadExtDummy checks
  this->m_pExtDummy_Town = nullptr; 

  if (!this->_LoadBspSec(pszMapCode)) return false;

  for (int j = 0; j < pMapSet->m_nLayerNum; ++j)
    this->m_ls[j].CreateLayer(this->m_SecInfo.m_nSecNum);

  if (!this->_LoadMonBlk(pszMapCode, pMapSet)) return false;
  if (!this->_LoadPortal(pszMapCode)) return false;
  if (!this->_LoadStoreDummy(pszMapCode)) return false;
  if (!this->_LoadStart(pszMapCode)) return false;
  if (!this->_LoadResource(pszMapCode)) return false;
  if (!this->_LoadBind(pszMapCode)) return false;
  if (!this->_LoadQuest(pszMapCode)) return false;
  if (!this->_LoadSafe(pszMapCode)) return false;

  this->m_tmrMineGradeReSet.BeginTimer(1800000); // 30 mins
  this->m_bLoad = true;

  if (!pMapSet->m_nMapType)
      this->m_ls[0].ActiveLayer(this->m_mb);

  return true;
}

bool CMapData::ConvertLocalToWorldDummy(CDummyPosTable *pPosTable, bool bCheckCenter)
{
  int RecordNum = pPosTable->GetRecordNum();
  for (int i = 0; i < RecordNum; ++i)
  {
    _dummy_position *pPos = (_dummy_position*)pPosTable->GetRecord(i);
    if (!this->ConvertLocal(pPos))
      return false;
    if (bCheckCenter)
      this->CheckCenterPosDummy(pPos);
  }
  return true;
}

bool CMapData::_LoadBspSec(char *pszMapCode)
{
  if (this->m_Level.mIsLoadedBsp)
  {
    MyMessageBox("CMapData Error", "m_Level.IsLoadedBsp() == false");
    return false;
  }

  char Buffer[256];
  sprintf_s(Buffer, ".\\Map\\%s\\%s.bsp", pszMapCode, pszMapCode);
  this->m_Level.LoadLevel(Buffer);
  
  strcpy_s(this->m_BspInfo.m_szBspName, pszMapCode);
  this->m_BspInfo.m_nLeafNum = this->m_Level.mBsp->mLeafNum;

  for (int j = 0; j < 3; ++j)
  {
    this->m_BspInfo.m_nMapMaxSize[j] = this->m_Level.mBsp->mNode[1].bb_max[j];
    this->m_BspInfo.m_nMapMinSize[j] = this->m_Level.mBsp->mNode[1].bb_min[j];
    this->m_BspInfo.m_nMapSize[j] = this->m_BspInfo.m_nMapMaxSize[j] - this->m_BspInfo.m_nMapMinSize[j];
  }
  this->m_SecInfo.m_nSecNumW = this->m_BspInfo.m_nMapSize[0] / 100 + 1;
  this->m_SecInfo.m_nSecNumH = this->m_BspInfo.m_nMapSize[2] / 100 + 1;
  this->m_SecInfo.m_nSecNum = this->m_SecInfo.m_nSecNumH * this->m_SecInfo.m_nSecNumW;
  return true;
}

bool CMapData::ConvertLocal(_dummy_position *pPos)
{
    float v11[3], v12[3], v13[3], v14[3], v15[3];
    for(int i=0; i<3; ++i) {
        v11[i] = (float)pPos->m_zLocalMin[i];
        v12[i] = (float)pPos->m_zLocalMax[i];
    }

    if (!this->m_Dummy.GetWorldFromLocal(pPos->m_fMin, pPos->m_wLineIndex, v11)) return false;
    if (!this->m_Dummy.GetWorldFromLocal(pPos->m_fMax, pPos->m_wLineIndex, v12)) return false;

    v13[0] = (float)pPos->m_zLocalMax[0];
    v13[1] = (float)pPos->m_zLocalMin[1];
    v13[2] = (float)pPos->m_zLocalMin[2];

    v14[0] = (float)pPos->m_zLocalMin[0];
    v14[1] = (float)pPos->m_zLocalMax[1];
    v14[2] = (float)pPos->m_zLocalMax[2];

    if (!this->m_Dummy.GetWorldFromLocal(pPos->m_fRT, pPos->m_wLineIndex, v13)) return false;
    if (!this->m_Dummy.GetWorldFromLocal(pPos->m_fLB, pPos->m_wLineIndex, v14)) return false;

    v15[0] = (float)((pPos->m_zLocalMax[0] - pPos->m_zLocalMin[0]) / 2 + pPos->m_zLocalMin[0]);
    v15[1] = (float)((pPos->m_zLocalMax[1] - pPos->m_zLocalMin[1]) / 2 + pPos->m_zLocalMin[1]);
    v15[2] = (float)pPos->m_zLocalMin[2];

    if (!this->m_Dummy.GetWorldFromLocal(pPos->m_fDirection, pPos->m_wLineIndex, v15)) return false;

    return true;
}

bool CMapData::CheckCenterPosDummy(_dummy_position *pPos)
{
    float v7[3];
    for (int j = 0; j < 3; ++j)
        v7[j] = (float)((pPos->m_zLocalMin[j] + pPos->m_zLocalMax[j]) / 2);

    if (this->m_Dummy.GetWorldFromLocal(pPos->m_fCenterPos, pPos->m_wLineIndex, v7))
    {
        pPos->m_fCenterPos[1] = this->m_Level.GetFirstYpos(pPos->m_fCenterPos, pPos->m_fMin, pPos->m_fMax);
        pPos->m_bPosAble = (pPos->m_fCenterPos[1] != -65535.0f);
        pPos->m_fDirection[1] = pPos->m_fCenterPos[1];
        return true;
    }
    return false;
}

bool CMapData::GetRandPosInDummy(_dummy_position *pPos, float *pNewPos, bool bRePos)
{
    if (!pPos->m_bPosAble)
        return false;

    float v10[3];
    for (int j = 0; j < 3; ++j)
    {
        float v8[3];
        v8[0] = (float)(pPos->m_zLocalMin[0] + rand() % (pPos->m_zLocalMax[0] - pPos->m_zLocalMin[0]));
        v8[2] = (float)(pPos->m_zLocalMin[2] + rand() % (pPos->m_zLocalMax[2] - pPos->m_zLocalMin[2]));
        v8[1] = (float)((pPos->m_zLocalMin[1] + pPos->m_zLocalMax[1]) / 2);

        if (!this->m_Dummy.GetWorldFromLocal(pNewPos, pPos->m_wLineIndex, v8))
        {
            MyMessageBox("CMapData Error", "GetRandPosInDummy map:%s, dummy:%s", this->m_pMapSet->m_strCode, pPos->m_szCode);
            return false;
        }

        pNewPos[1] = this->m_Level.GetFirstYpos(pNewPos, pPos->m_fMin, pPos->m_fMax);
        if (pNewPos[1] != -65535.0f)
        {
            float v11_y = pNewPos[1];
        if (pNewPos[1] != -65535.0f)
        {
            float v11_y = pNewPos[1];
            pNewPos[1] = pPos->m_fCenterPos[1];
            
            float v10_temp[3];
            if (this->m_Level.mBsp->CanYouGoThere(pPos->m_fCenterPos, pNewPos, (float(*)[3])v10_temp) || bRePos)
            {
                if (bRePos)
                {
                    pNewPos[0] = v10_temp[0];
                    pNewPos[2] = v10_temp[2];
                    pNewPos[1] = this->m_Level.GetFirstYpos(pNewPos, pPos->m_fMin, pPos->m_fMax);
                    if (pNewPos[1] == -65535.0f) continue;
                }
                else
                {
                   pNewPos[1] = v11_y;
                }
                return true;
            }
        }
        }
    }
    memcpy(pNewPos, pPos->m_fCenterPos, 12);
    return true;
}

bool CMapData::IsMapIn(float *fPos)
{
  for (int j = 0; j < 3; ++j)
  {
    if (fPos[j] > static_cast<float>(m_BspInfo.m_nMapMaxSize[j])
        || static_cast<float>(m_BspInfo.m_nMapMinSize[j]) > fPos[j])
    {
      return false;
    }
  }
  return true;
}

bool CMapData::_LoadMonBlk(char *pszMapCode, _map_fld *pMapFld)
{
    char pszErrMsg[160];
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    
    if (!this->m_tbMonDumPos.LoadDummyPosition(Buffer, "*dm"))
    {
        MyMessageBox("CMapData Error", "m_tbMonDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }

    if (!this->ConvertLocalToWorldDummy(&this->m_tbMonDumPos, true)) return false;

    if (pMapFld->m_nMapType == 2)
    {
        this->m_nMonBlockNum = 0;
        return true;
    }

    sprintf_s(Buffer, ".\\Map\\%s\\%s-[BLOCK].dat", pszMapCode, pszMapCode);
    if (!this->m_tbMonBlk.ReadRecord(Buffer, 0x5A4, pszErrMsg))
    {
        this->m_nMonBlockNum = 0;
        return true;
    }

    this->m_nMonBlockNum = this->m_tbMonBlk.GetRecordNum();
    if (this->m_nMonBlockNum > 200)
    {
        MyMessageBox("CMapData Error", "%s: m_nMonBlockNum(%d) > max_block_num(%d)", pszMapCode, this->m_nMonBlockNum, 200);
        return false;
    }

    this->m_pMonBlock = new _mon_block[this->m_nMonBlockNum];
    for (int n = 0; n < this->m_nMonBlockNum; ++n)
    {
        _mon_block_fld *pBlkRec = (_mon_block_fld *)this->m_tbMonBlk.GetRecord(n);
        if (!pBlkRec) return false;

        this->m_nMonDumNum += pBlkRec->m_dwDummyNum;
        _dummy_position *ppPos[256]; // Temporary fixed size for loading

        for (unsigned int j = 0; j < pBlkRec->m_dwDummyNum; ++j)
        {
            if (strcmp(pBlkRec->m_DummyInfo[j].m_strDummyCode, "0") == 0)
            {
                pBlkRec->m_dwDummyNum = j;
                break;
            }
            ppPos[j] = (_dummy_position*)this->m_tbMonDumPos.GetRecord(pBlkRec->m_DummyInfo[j].m_strDummyCode);
            if (!ppPos[j]) return false;
        }

        if (!this->m_pMonBlock[n].SetBlock(pBlkRec, this, ppPos)) return false;
        bool IsRotate = g_MonsterSetInfoData.IsRotateBlock(&this->m_pMonBlock[n]);
        this->m_pMonBlock[n].SetRotateBlock(IsRotate);
    }

    if (pMapFld->m_nMonsterSetFileNum > 30) return false;

    for (int k = 0; k < pMapFld->m_nMonsterSetFileNum; ++k)
    {
        this->m_mb[k].m_nBlockNum = this->m_nMonBlockNum;
        this->m_mb[k].m_ptbMonBlock = new CRecordData[this->m_nMonBlockNum];
        this->m_mb[k].m_ppMonAct = new _mon_active*[this->m_nMonBlockNum];

        for (int m = 0; m < this->m_nMonBlockNum; ++m)
            this->m_mb[k].m_ppMonAct[m] = nullptr;

        for (int n = 0; n < this->m_nMonBlockNum; ++n)
        {
            _mon_block_fld *pBlkRec = (_mon_block_fld *)this->m_tbMonBlk.GetRecord(n);
            if (!pBlkRec) return false;

            if (pMapFld->m_nMapType == 1)
                sprintf_s(Buffer, ".\\Map\\%s\\%s_%d.dat", pszMapCode, pBlkRec->m_strCode, k);
            else
                sprintf_s(Buffer, ".\\Map\\%s\\%s.dat", pszMapCode, pBlkRec->m_strCode);

            if (!this->m_mb[k].m_ptbMonBlock[n].ReadRecord(Buffer, 0x5C, pszErrMsg))
            {
                MyMessageBox("CMapData Error", pszErrMsg);
                return false;
            }

            int ActNum = this->m_mb[k].m_ptbMonBlock[n].GetRecordNum();
            if (ActNum > 300) return false;

            this->m_mb[k].m_ppMonAct[n] = new _mon_active[ActNum];
            for (int ii = 0; ii < ActNum; ++ii)
            {
                _mon_active_fld *pActRec = (_mon_active_fld *)this->m_mb[k].m_ptbMonBlock[n].GetRecord(ii);
                if (!pActRec) return false;

                _monster_fld *pMonRec = (_monster_fld *)g_Main.m_tblMonster.GetRecord(pActRec->m_strCode);
                if (!pMonRec) return false;

                if (!this->m_mb[k].m_ppMonAct[n][ii].SetActive(pActRec, &this->m_pMonBlock[n], pMonRec->m_dwIndex))
                    return false;

                this->m_nMonTotalCount += pActRec->m_dwRegenLimNum;
            }
        }
    }
    return true;
}
bool CMapData::_LoadPortal(char *pszMapCode)
{
    char Buffer[256];
    char pszErrMsg[160];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbPortalDumPos.LoadDummyPosition(Buffer, "*po")) return false;
    if (!this->ConvertLocalToWorldDummy(&this->m_tbPortalDumPos, true)) return false;

    sprintf_s(Buffer, ".\\Map\\%s\\%s-[PORTAL].dat", pszMapCode, pszMapCode);
    if (!this->m_tbPortal.ReadRecord(Buffer, 0x48, pszErrMsg))
    {
        this->m_nPortalNum = 0;
        return true;
    }

    this->m_nPortalNum = this->m_tbPortal.GetRecordNum();
    this->m_pPortal = new _portal_dummy[this->m_nPortalNum];
    for (int i = 0; i < this->m_nPortalNum; ++i)
    {
        _portal_fld *pRec = (_portal_fld *)this->m_tbPortal.GetRecord(i);
        if (!pRec) return false;
        this->m_pPortal[i].m_pPortalRec = pRec;
        this->m_pPortal[i].m_pDumPos = (_dummy_position*)this->m_tbPortalDumPos.GetRecord(pRec->m_strCode);
    }
    return true;
}

bool CMapData::_LoadStoreDummy(char *pszMapCode)
{
    char Buffer[256];
    char pszErrMsg[160];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbStoreDumPos.LoadDummyPosition(Buffer, "*st")) return false;
    if (!this->ConvertLocalToWorldDummy(&this->m_tbStoreDumPos, true)) return false;

    this->m_nItemStoreDumNum = this->m_tbStoreDumPos.GetRecordNum();
    this->m_pItemStoreDummy = new _store_dummy[this->m_nItemStoreDumNum];
    for (int i = 0; i < this->m_nItemStoreDumNum; ++i)
    {
        _dummy_position *pPos = (_dummy_position*)this->m_tbStoreDumPos.GetRecord(i);
        this->m_pItemStoreDummy[i].m_pDumPos = pPos;
        this->m_pItemStoreDummy[i].m_pStoreRec = nullptr; // To be linked later if needed
        this->m_pItemStoreDummy[i].m_nStoreType = 0;
    }
    return true;
}

bool CMapData::_LoadStart(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbStartDumPos.LoadDummyPosition(Buffer, "*sa")) return false;
    if (!this->ConvertLocalToWorldDummy(&this->m_tbStartDumPos, true)) return false;

    this->m_nStartDumNum = this->m_tbStartDumPos.GetRecordNum();
    this->m_pStartDummy = new _start_dummy[this->m_nStartDumNum];
    for (int i = 0; i < this->m_nStartDumNum; ++i)
    {
        this->m_pStartDummy[i].m_pDumPos = (_dummy_position*)this->m_tbStartDumPos.GetRecord(i);
    }
    return true;
}

bool CMapData::_LoadResource(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    this->m_tbResDumPosHigh.LoadDummyPosition(Buffer, "*rh");
    this->m_tbResDumPosMiddle.LoadDummyPosition(Buffer, "*rm");
    this->m_tbResDumPosLow.LoadDummyPosition(Buffer, "*rl");
    
    this->ConvertLocalToWorldDummy(&this->m_tbResDumPosHigh, true);
    this->ConvertLocalToWorldDummy(&this->m_tbResDumPosMiddle, true);
    this->ConvertLocalToWorldDummy(&this->m_tbResDumPosLow, true);

    this->m_nResDumNum = this->m_tbResDumPosHigh.GetRecordNum() + this->m_tbResDumPosMiddle.GetRecordNum() + this->m_tbResDumPosLow.GetRecordNum();
    this->m_pResDummy = new _res_dummy[this->m_nResDumNum];
    // Simplified: Just allocating for now, original has complex resource link logic
    return true;
}

bool CMapData::_LoadBind(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbBindDumPos.LoadDummyPosition(Buffer, "*bi")) return false;
    if (!this->ConvertLocalToWorldDummy(&this->m_tbBindDumPos, true)) return false;

    this->m_nBindDumNum = this->m_tbBindDumPos.GetRecordNum();
    this->m_pBindDummy = new _bind_dummy[this->m_nBindDumNum];
    for (int i = 0; i < this->m_nBindDumNum; ++i)
    {
        this->m_pBindDummy[i].m_pDumPos = (_dummy_position*)this->m_tbBindDumPos.GetRecord(i);
    }
    return true;
}

bool CMapData::_LoadQuest(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbQuestDumPos.LoadDummyPosition(Buffer, "*qu")) return false;
    if (!this->ConvertLocalToWorldDummy(&this->m_tbQuestDumPos, true)) return false;

    this->m_nQuestDumNum = this->m_tbQuestDumPos.GetRecordNum();
    this->m_pQuestDummy = new _quest_dummy[this->m_nQuestDumNum];
    for (int i = 0; i < this->m_nQuestDumNum; ++i)
    {
        this->m_pQuestDummy[i].m_pDumPos = (_dummy_position*)this->m_tbQuestDumPos.GetRecord(i);
    }
    return true;
}

bool CMapData::_LoadSafe(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbSafeDumPos.LoadDummyPosition(Buffer, "*sf")) return false;
    if (!this->ConvertLocalToWorldDummy(&this->m_tbSafeDumPos, true)) return false;

    this->m_nSafeDumNum = this->m_tbSafeDumPos.GetRecordNum();
    this->m_pSafeDummy = new _safe_dummy[this->m_nSafeDumNum];
    for (int i = 0; i < this->m_nSafeDumNum; ++i)
    {
        this->m_pSafeDummy[i].m_pDumPos = (_dummy_position*)this->m_tbSafeDumPos.GetRecord(i);
    }
    return true;
}

_portal_dummy *CMapData::GetPortal(int nPortalIndex)
{
    if (nPortalIndex < 0 || nPortalIndex >= this->m_nPortalNum) return nullptr;
    return &this->m_pPortal[nPortalIndex];
}

_portal_dummy *CMapData::GetPortal(char *pPortalCode)
{
    for (int j = 0; j < this->m_nPortalNum; ++j)
    {
        if (strcmp(pPortalCode, this->m_pPortal[j].m_pPortalRec->m_strCode) == 0)
            return &this->m_pPortal[j];
    }
    return nullptr;
}
