#include "pch.h"

#include "CMapData.h"

#include <cstring>
#include <new>
#include "CGameObject.h"
#include "CExtDummy.h"
#include "CItemStoreManager.h"
#include "CLevel.h"
#include "MonsterSetInfoData.h"
#include "pnt_rect.h"
#include "StoreList_fld.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

void CMapData::Init(_map_fld *pMapSet)
{
  this->m_nMapCode = pMapSet->m_dwIndex;
  this->m_pMapSet = pMapSet;
}

bool CMapData::OpenMap(char *pszMapCode, _map_fld *pMapSet, bool bUse)
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
  this->m_bUse = bUse;
  this->m_nMapIndex = pMapSet->m_dwIndex;

  char Buffer[256];
  sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
  if (!this->m_Dummy.LoadExtDummy(Buffer))
  {
      MyMessageBox("CMapData Error", "read dummy-list fail(%s)", pszMapCode);
      return false;
  }

  sprintf_s(Buffer, ".\\map\\%s\\%sEXT.spt", pszMapCode, pszMapCode);
  this->m_pExtDummy_Town = nullptr;
  if (CalcFileSize(Buffer) > 0)
  {
      this->m_pExtDummy_Town = new (std::nothrow) CExtDummy();
      if (!this->m_pExtDummy_Town || !this->m_pExtDummy_Town->LoadExtDummy(Buffer))
      {
          if (this->m_pExtDummy_Town)
              delete this->m_pExtDummy_Town;
          this->m_pExtDummy_Town = nullptr;
          MyMessageBox("CMapData Error", "read town-dummy fail(%s)", pszMapCode);
          return false;
      }
  }

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

bool CMapData::LoadDummy(char *pszDummyCode, _dummy_position *pPos)
{
  char buffer[144]{};
  sprintf(buffer, ".\\map\\%s\\%s.spt", m_pMapSet->m_strCode, m_pMapSet->m_strCode);
  if (CDummyPosTable::FindDummy(buffer, pszDummyCode, pPos))
  {
    return ConvertLocal(pPos) && CheckCenterPosDummy(pPos);
  }
  MyMessageBox("CMapData Error", "CDummyPosTable::FindDummy(%s, %s) == false", m_pMapSet->m_strCode, pszDummyCode);
  return false;
}

bool CMapData::LoadHolySystemDummy(char *pszDummyCode, _dummy_position *pPos)
{
  char buffer[144]{};
  sprintf_s(buffer, ".\\map\\%s\\%s.spt", m_pMapSet->m_strCode, m_pMapSet->m_strCode);

  if (CDummyPosTable::FindDummy(buffer, pszDummyCode, pPos))
  {
    return ConvertLocal(pPos) && CheckCenterPosDummy(pPos);
  }

  MyMessageBox(
    "CMapData Error",
    "CDummyPosTable::FindDummy(%s, %s) == false",
    m_pMapSet->m_strCode,
    pszDummyCode);
  return false;
}

_dummy_position *CMapData::GetDummyPostion(char *pszDummyCode)
{

  if (!strncmp(pszDummyCode, "dm", 2u))
    return static_cast<_dummy_position *>(m_tbMonDumPos.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "dp", 2u))
    return static_cast<_dummy_position *>(m_tbPortalDumPos.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "sd", 2u))
    return static_cast<_dummy_position *>(m_tbStoreDumPos.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "ds", 2u))
    return static_cast<_dummy_position *>(m_tbStartDumPos.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "bd", 2u))
    return static_cast<_dummy_position *>(m_tbBindDumPos.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "0dr", 3u))
    return static_cast<_dummy_position *>(m_tbResDumPosHigh.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "1dr", 3u))
    return static_cast<_dummy_position *>(m_tbResDumPosMiddle.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "2dr", 3u))
    return static_cast<_dummy_position *>(m_tbResDumPosLow.GetRecord(pszDummyCode));
  if (!strncmp(pszDummyCode, "dq", 2u))
    return static_cast<_dummy_position *>(m_tbQuestDumPos.GetRecord(pszDummyCode));
  return nullptr;
}

bool CMapData::GetRandPosInDummy(_dummy_position *pPos, float *pNewPos, bool bRePos)
{
    if (!pPos->m_bPosAble)
        return false;

    for (int j = 0; j < 3; ++j)
    {
        float localPos[3];
        localPos[0] = (float)(pPos->m_zLocalMin[0] + rand() % (pPos->m_zLocalMax[0] - pPos->m_zLocalMin[0]));
        localPos[2] = (float)(pPos->m_zLocalMin[2] + rand() % (pPos->m_zLocalMax[2] - pPos->m_zLocalMin[2]));
        localPos[1] = (float)((pPos->m_zLocalMin[1] + pPos->m_zLocalMax[1]) / 2);

        if (!this->m_Dummy.GetWorldFromLocal(pNewPos, pPos->m_wLineIndex, localPos))
        {
            MyMessageBox("CMapData Error", "GetRandPosInDummy map:%s, dummy:%s", this->m_pMapSet->m_strCode, pPos->m_szCode);
            return false;
        }

        pNewPos[1] = this->m_Level.GetFirstYpos(pNewPos, pPos->m_fMin, pPos->m_fMax);
        if (pNewPos[1] != -65535.0f)
        {
            float originalY = pNewPos[1];
            pNewPos[1] = pPos->m_fCenterPos[1];

            float crossPoint[3];
            bool canUse = this->m_Level.mBsp->CanYouGoThere(
                pPos->m_fCenterPos,
                pNewPos,
                (float(*)[3])crossPoint);

            if (!canUse && bRePos)
            {
                pNewPos[0] = crossPoint[0];
                pNewPos[2] = crossPoint[2];
                pNewPos[1] = this->m_Level.GetFirstYpos(pNewPos, pPos->m_fMin, pPos->m_fMax);
                canUse = (pNewPos[1] != -65535.0f);
            }

            if (canUse)
            {
                pNewPos[1] = originalY;
                return true;
            }
        }
    }
    memcpy(pNewPos, pPos->m_fCenterPos, 12);
    return true;
}

__int64 CMapData::GetResDummySector(int nDummyIndex, float *pCurPos)
{
  if (m_nResDumNum <= 0)
  {
    return 0xFFFFFFFFLL;
  }

  _res_dummy *dummy = &m_pResDummy[nDummyIndex];
  if (!m_Dummy.IsInBBox(dummy->m_pDumPos->m_wLineIndex, pCurPos))
  {
    return 0xFFFFFFFFLL;
  }

  unsigned int sectorIndex = static_cast<unsigned int>(-1);
  if (dummy->GetQualityGrade())
  {
    return 100;
  }

  _dummy_position *dumPos = dummy->m_pDumPos;
  if (!m_Dummy.mDummy || !m_Dummy.mNum || dumPos->m_wLineIndex >= m_Dummy.mNum)
  {
    return 0xFFFFFFFFLL;
  }

  _EXT_DUMMY *extDummy = &m_Dummy.mDummy[dumPos->m_wLineIndex];
  for (int j = 0; j < 3; ++j)
  {
    memcpy_0(extDummy->mBBmin, dummy->m_fMinLocal[j], sizeof(extDummy->mBBmin));
    memcpy_0(extDummy->mBBmax, dummy->m_fMaxLocal[j], sizeof(extDummy->mBBmax));
    if (m_Dummy.IsInBBox(dumPos->m_wLineIndex, pCurPos))
    {
      sectorIndex = static_cast<unsigned int>(j);
      break;
    }
  }
  memcpy_0(extDummy->mBBmin, dummy->m_fMinLocal[2], sizeof(extDummy->mBBmin));
  memcpy_0(extDummy->mBBmax, dummy->m_fMaxLocal[2], sizeof(extDummy->mBBmax));
  return sectorIndex;
}

bool CMapData::GetRandPosInRange(float *pStdPos, int nRange, float *pNewPos)
{
  float crossPoint[3];
  for (int j = 0; j <= 50; ++j)
  {
    const float xMin = pStdPos[0] - static_cast<float>(nRange / 2);
    pNewPos[0] = xMin + static_cast<float>(rand() % nRange);
    const float zMin = pStdPos[2] - static_cast<float>(nRange / 2);
    pNewPos[2] = zMin + static_cast<float>(rand() % nRange);
    pNewPos[1] = pStdPos[1];
    if (static_cast<unsigned int>(
          this->m_Level.mBsp->CanYouGoThere(pStdPos, pNewPos, reinterpret_cast<float(*)[3]>(crossPoint))))
    {
      return true;
    }
  }
  return false;
}

bool CMapData::GetRandPosVirtualDumExcludeStdRange(float *pStdPos, int nRange, int iExcludeRange, float *pNewPos)
{
  float minPos[3]{};
  float maxPos[3]{};
  minPos[0] = pStdPos[0] - static_cast<float>(nRange / 2);
  minPos[1] = pStdPos[1] - 100.0f;
  minPos[2] = pStdPos[2] - static_cast<float>(nRange / 2);
  maxPos[0] = pStdPos[0] + static_cast<float>(nRange / 2);
  maxPos[1] = pStdPos[1] + 100.0f;
  maxPos[2] = pStdPos[2] + static_cast<float>(nRange / 2);

  const float excludeMinX = pStdPos[0] - static_cast<float>(iExcludeRange / 2);
  const float excludeMaxX = pStdPos[0] + static_cast<float>(iExcludeRange / 2);
  const float excludeMinZ = pStdPos[2] - static_cast<float>(iExcludeRange / 2);
  const float excludeMaxZ = pStdPos[2] + static_cast<float>(iExcludeRange / 2);

  int failCount = 0;
  while (true)
  {
    while (true)
    {
      do
      {
        const float xMin = pStdPos[0] - static_cast<float>(nRange / 2);
        pNewPos[0] = xMin + static_cast<float>(rand() % nRange);
        const float zMin = pStdPos[2] - static_cast<float>(nRange / 2);
        pNewPos[2] = zMin + static_cast<float>(rand() % nRange);
      } while (pNewPos[0] >= excludeMinX
               && excludeMaxX >= pNewPos[0]
               && pNewPos[2] >= excludeMinZ
               && excludeMaxZ >= pNewPos[2]);

      pNewPos[1] = this->m_Level.mBsp->GetFirstYpos(pNewPos, minPos, maxPos);
      if (pNewPos[1] != -65535.0f)
      {
        break;
      }
      if (failCount++ > 50)
      {
        return false;
      }
    }

    float crossPoint[3]{};
    if (static_cast<unsigned int>(
          this->m_Level.mBsp->CanYouGoThere(pStdPos, pNewPos, reinterpret_cast<float(*)[3]>(crossPoint))))
    {
      break;
    }
    if (failCount++ > 50)
    {
      return false;
    }
  }
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
        if (!pBlkRec)
        {
            MyMessageBox("CMapData Error", "(_mon_block_fld*)m_tbMonBlk.GetRecord(%d) == NULL", n);
            return false;
        }

        if (pBlkRec->m_dwDummyNum > 0xFFFF)
        {
            MyMessageBox(
              "CMapData Error",
              "_LoadMonBlk(%s).. if(pBlkRec(%d)->m_dwDummyNum(%d) > 0xFFFF)",
              pszMapCode,
              pBlkRec->m_dwIndex,
              pBlkRec->m_dwDummyNum);
            return false;
        }

        this->m_nMonDumNum += pBlkRec->m_dwDummyNum;
        _dummy_position *ppPos[256]; // Temporary fixed size for loading

        for (unsigned int j = 0; j < pBlkRec->m_dwDummyNum; ++j)
        {
            if (strcmp(pBlkRec->m_DummyInfo[j].m_strDummyCode, "0") == 0)
            {
                g_Main.m_logLoadingError.Write(
                  "Map:%s, MonBlock:%s.. dummy(alter %d->%d)",
                  pszMapCode,
                  pBlkRec->m_strCode,
                  pBlkRec->m_dwDummyNum,
                  j);
                pBlkRec->m_dwDummyNum = j;
                break;
            }
            ppPos[j] = (_dummy_position*)this->m_tbMonDumPos.GetRecord(pBlkRec->m_DummyInfo[j].m_strDummyCode);
            if (!ppPos[j])
            {
                MyMessageBox(
                  "CMapData Error",
                  "Map ( %s ) blk ( %s ) m_tbMonDumPos.GetRecord(%s) == NULL",
                  pszMapCode,
                  pBlkRec->m_strCode,
                  pBlkRec->m_DummyInfo[j].m_strDummyCode);
                return false;
            }
        }

        if (!this->m_pMonBlock[n].SetBlock(pBlkRec, this, ppPos))
        {
            MyMessageBox(
              "CMapData Error",
              "%s: m_pMonBlock[%d].SetBlock(%s) == false",
              pszMapCode,
              n,
              pBlkRec->m_strCode);
            return false;
        }
        bool IsRotate = g_MonsterSetInfoData.IsRotateBlock(&this->m_pMonBlock[n]);
        this->m_pMonBlock[n].SetRotateBlock(IsRotate);
    }

    if (pMapFld->m_nMonsterSetFileNum > 30)
    {
        MyMessageBox(
          "CMapData Error",
          "%s: m_nMonsterSetFileNum(%d) > max_monster_set_file_num(%d)",
          pszMapCode,
          pMapFld->m_nMonsterSetFileNum,
          30);
        return false;
    }

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
            if (ActNum > 300)
            {
                MyMessageBox("CMapData Error", "%s: nActNum(%d) > max_act_num(%d)", pszMapCode, ActNum, 300);
                return false;
            }

            this->m_mb[k].m_ppMonAct[n] = new _mon_active[ActNum];
            for (int ii = 0; ii < ActNum; ++ii)
            {
                _mon_active_fld *pActRec = (_mon_active_fld *)this->m_mb[k].m_ptbMonBlock[n].GetRecord(ii);
                if (!pActRec)
                {
                    MyMessageBox(
                      "CMapData Error",
                      "(_mon_dummy_fld*)m_ptbMonDum[%d]->GetRecord(%d) == NULL",
                      n,
                      ii);
                    return false;
                }

                _monster_fld *pMonRec = (_monster_fld *)g_Main.m_tblMonster.GetRecord(pActRec->m_strCode);
                if (!pMonRec)
                {
                    MyMessageBox(
                      "CMapData Error",
                      "(_monster_record*)g_Main.m_tblMonsterCharacter.GetRecord(%s) == NULL",
                      pActRec->m_strCode);
                    return false;
                }

                if (!this->m_mb[k].m_ppMonAct[n][ii].SetActive(pActRec, &this->m_pMonBlock[n], pMonRec->m_dwIndex))
                {
                    MyMessageBox(
                      "CMapData Error",
                      "m_ppMonDummy[%d][%d].SetDummy(pRec, pPos, &m_pMonBlock[%d]) == false",
                      n,
                      ii,
                      n);
                    return false;
                }

                this->m_nMonTotalCount += pActRec->m_dwRegenLimNum;
            }
        }
    }
    return true;
}
bool CMapData::_LoadPortal(char *pszMapCode)
{
    char pszErrMsg[160];
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbPortalDumPos.LoadDummyPosition(Buffer, "*dp"))
    {
        MyMessageBox("CMapData Error", "m_tbPortalDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbPortalDumPos, false)) return false;

    sprintf_s(Buffer, ".\\map\\%s\\%s-[Portal].dat", pszMapCode, pszMapCode);
    if (!this->m_tbPortal.ReadRecord(Buffer, 0x178, pszErrMsg))
    {
        MyMessageBox("CMapData Error", pszErrMsg);
        return false;
    }

    this->m_nPortalNum = this->m_tbPortal.GetRecordNum();
    this->m_pPortal = new _portal_dummy[this->m_nPortalNum];
    for (int n = 0; n < this->m_nPortalNum; ++n)
    {
        _portal_fld *pRec = (_portal_fld *)this->m_tbPortal.GetRecord(n);
        if (!pRec)
        {
            MyMessageBox(
              "CMapData-LoadPortal(LPTSTR pszMapCode) Error",
              "Map(%s): (_portal_record*)m_tbPortal.GetRecord(%d) = NULL",
              pszMapCode,
              n);
            return false;
        }
        _dummy_position *pDumPos = (_dummy_position*)this->m_tbPortalDumPos.GetRecord(pRec->m_strCode);
        if (!pDumPos)
        {
            MyMessageBox(
              "CMapData-LoadPortal(LPTSTR pszMapCode) Error",
              "Map(%s): tbPortalPos.GetRecord(%s) = NULL",
              pszMapCode,
              pRec->m_strCode);
            g_Main.m_logLoadingError.Write(
              "Map(%s): tbPortalPos.GetRecord(%s) = NULL",
              pszMapCode,
              pRec->m_strCode);
            return false;
        }

        this->m_pPortal[n].m_pPortalRec = pRec;
        this->m_pPortal[n].m_pDumPos = pDumPos;
        this->CheckCenterPosDummy(pDumPos);
    }
    return true;
}

bool CMapData::_LoadStoreDummy(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbStoreDumPos.LoadDummyPosition(Buffer, "*sd"))
    {
        MyMessageBox("CMapData Error", "m_tbStoreDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbStoreDumPos, false)) return false;

    CRecordData *pStoreTable = &CItemStoreManager::Instance()->m_tblItemStore;
    this->m_nItemStoreDumNum = 0;
    for (int n = 0; n < pStoreTable->GetRecordNum(); ++n)
    {
        _StoreList_fld *pRec = (_StoreList_fld *)pStoreTable->GetRecord(n);
        if (pRec && strcmp(pRec->m_strStore_MAPcode, pszMapCode) == 0)
            ++this->m_nItemStoreDumNum;
    }

    if (this->m_tbStoreDumPos.GetRecordNum())
    {
        if (this->m_nItemStoreDumNum > 0)
        {
            this->m_pItemStoreDummy = new _store_dummy[this->m_nItemStoreDumNum];
            int v13 = 0;
            for (int n = 0; n < pStoreTable->GetRecordNum(); ++n)
            {
                _StoreList_fld *pRec = (_StoreList_fld *)pStoreTable->GetRecord(n);
                if (pRec && strcmp(pRec->m_strStore_MAPcode, pszMapCode) == 0)
                {
                    _dummy_position *pDumPos = (_dummy_position*)this->m_tbStoreDumPos.GetRecord(pRec->m_strCode);
                    if (!pDumPos)
                    {
                        MyMessageBox(
                          "CMapData-LoadStore(LPTSTR pszMapCode) Error",
                          "Map(%s): m_tbStoreDumPos.GetRecord(%s) = NULL",
                          pszMapCode,
                          pRec->m_strCode);
                        return false;
                    }
                    this->m_pItemStoreDummy[v13].m_nStoreType = 0;
                    this->m_pItemStoreDummy[v13].m_pStoreRec = pRec;
                    this->m_pItemStoreDummy[v13].m_pDumPos = pDumPos;
                    this->CheckCenterPosDummy(pDumPos);
                    ++v13;
                }
            }
        }

        for (int j = 0; j < 3; ++j)
        {
            if (!CDummyPosTable::ms_pHeroes_Dummy[j])
            {
                _dummy_position *pHero = (_dummy_position*)this->m_tbStoreDumPos.GetRecord(C_HEROES_DUMMY_NAME[j]);
                CDummyPosTable::ms_pHeroes_Dummy[j] = pHero;
            }
        }
        return true;
    }

    this->m_nItemStoreDumNum = 0;
    return true;
}

bool CMapData::_LoadStart(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbStartDumPos.LoadDummyPosition(Buffer, "*ds"))
    {
        MyMessageBox("CMapData Error", "m_tbStartDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbStartDumPos, false)) return false;

    this->m_nStartDumNum = this->m_tbStartDumPos.GetRecordNum();
    if (this->m_pMapSet->m_nMapClass == 1 && this->m_nStartDumNum < 3)
    {
        MyMessageBox(
          "CMapData Error",
          "Map[%s]-m_tbStartDumPos.GetRecordNum()[%d] > 0",
          pszMapCode,
          this->m_nStartDumNum);
        return false;
    }
    if (this->m_pMapSet->m_nRaceVillageCode < 3 && this->m_nStartDumNum == 0)
    {
        MyMessageBox(
          "CMapData Error",
          "Map[%s]-m_tbStartDumPos.GetRecordNum()[%d] > 0",
          pszMapCode,
          this->m_nStartDumNum);
        return false;
    }
    if (this->m_pMapSet->m_nMapType == 2 && this->m_nStartDumNum == 0)
    {
        MyMessageBox(
          "CMapData Error",
          "Map[%s]-m_tbStartDumPos.GetRecordNum()[%d] > 0",
          pszMapCode,
          this->m_nStartDumNum);
        return false;
    }

    if (this->m_nStartDumNum)
    {
        this->m_pStartDummy = new _start_dummy[this->m_nStartDumNum];
        for (int i = 0; i < this->m_nStartDumNum; ++i)
        {
            _dummy_position *pDumPos = (_dummy_position*)this->m_tbStartDumPos.GetRecord(i);
            if (!pDumPos)
            {
                MyMessageBox(
                  "CMapData-LoadStart Error",
                  "Map(%s): m_tbStartDumPos.GetRecord(%d) = NULL",
                  pszMapCode,
                  i);
                return false;
            }
            this->m_pStartDummy[i].m_pDumPos = pDumPos;
            this->CheckCenterPosDummy(pDumPos);
        }
    }
    return true;
}

bool CMapData::_LoadResource(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbResDumPosHigh.LoadDummyPosition(Buffer, "*0dr"))
    {
        MyMessageBox("CMapData Error", "m_tbResDumPosHigh.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbResDumPosHigh, false)) return false;

    if (!this->m_tbResDumPosMiddle.LoadDummyPosition(Buffer, "*1dr"))
    {
        MyMessageBox("CMapData Error", "m_tbResDumPosMiddle.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbResDumPosMiddle, false)) return false;

    if (!this->m_tbResDumPosLow.LoadDummyPosition(Buffer, "*2dr"))
    {
        MyMessageBox("CMapData Error", "m_tbResDumPosLow.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbResDumPosLow, false)) return false;

    int nHigh = this->m_tbResDumPosHigh.GetRecordNum();
    int nMiddle = this->m_tbResDumPosMiddle.GetRecordNum();
    int nLow = this->m_tbResDumPosLow.GetRecordNum();
    this->m_nResDumNum = nHigh + nMiddle + nLow;
    if (!this->m_nResDumNum)
        return true;

    this->m_pResDummy = new _res_dummy[this->m_nResDumNum];
    int v12 = 0;
    for (int i = 0; i < nHigh; ++i)
    {
        _dummy_position *pDumPos = (_dummy_position*)this->m_tbResDumPosHigh.GetRecord(i);
        this->m_pResDummy[v12++].SetDummy(pDumPos, 0);
        this->CheckCenterPosDummy(pDumPos);
    }
    for (int i = 0; i < nMiddle; ++i)
    {
        _dummy_position *pDumPos = (_dummy_position*)this->m_tbResDumPosMiddle.GetRecord(i);
        this->m_pResDummy[v12++].SetDummy(pDumPos, 1);
        this->CheckCenterPosDummy(pDumPos);
    }
    for (int i = 0; i < nLow; ++i)
    {
        _dummy_position *pDumPos = (_dummy_position*)this->m_tbResDumPosLow.GetRecord(i);
        this->m_pResDummy[v12++].SetDummy(pDumPos, 2);
        this->CheckCenterPosDummy(pDumPos);
    }
    return true;
}

bool CMapData::_LoadBind(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbBindDumPos.LoadDummyPosition(Buffer, "*bd"))
    {
        MyMessageBox("CMapData Error", "m_tbBindDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbBindDumPos, false)) return false;

    this->m_nBindDumNum = this->m_tbBindDumPos.GetRecordNum();
    if (this->m_nBindDumNum)
    {
        this->m_pBindDummy = new _bind_dummy[this->m_nBindDumNum];
        for (int i = 0; i < this->m_nBindDumNum; ++i)
        {
            _dummy_position *pDumPos = (_dummy_position*)this->m_tbBindDumPos.GetRecord(i);
            this->m_pBindDummy[i].m_pDumPos = pDumPos;
            this->CheckCenterPosDummy(pDumPos);
        }
    }
    return true;
}

bool CMapData::_LoadQuest(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbQuestDumPos.LoadDummyPosition(Buffer, "*dq"))
    {
        MyMessageBox("CMapData Error", "m_tbQuestDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbQuestDumPos, false)) return false;

    this->m_nQuestDumNum = this->m_tbQuestDumPos.GetRecordNum();
    if (this->m_nQuestDumNum)
    {
        this->m_pQuestDummy = new _quest_dummy[this->m_nQuestDumNum];
        for (int i = 0; i < this->m_nQuestDumNum; ++i)
        {
            _dummy_position *pDumPos = (_dummy_position*)this->m_tbQuestDumPos.GetRecord(i);
            this->m_pQuestDummy[i].m_pDumPos = pDumPos;
            this->CheckCenterPosDummy(pDumPos);
        }
    }
    return true;
}

bool CMapData::_LoadSafe(char *pszMapCode)
{
    char Buffer[256];
    sprintf_s(Buffer, ".\\map\\%s\\%s.spt", pszMapCode, pszMapCode);
    if (!this->m_tbSafeDumPos.LoadDummyPosition(Buffer, "*sa"))
    {
        MyMessageBox("CMapData Error", "m_tbSafeDumPos.LoadDummyPosition(%s) == false", Buffer);
        return false;
    }
    if (!this->ConvertLocalToWorldDummy(&this->m_tbSafeDumPos, false)) return false;

    this->m_nSafeDumNum = this->m_tbSafeDumPos.GetRecordNum();
    if (this->m_nSafeDumNum)
    {
        this->m_pSafeDummy = new _safe_dummy[this->m_nSafeDumNum];
        for (int i = 0; i < this->m_nSafeDumNum; ++i)
        {
            _dummy_position *pDumPos = (_dummy_position*)this->m_tbSafeDumPos.GetRecord(i);
            this->m_pSafeDummy[i].m_pDumPos = pDumPos;
            this->CheckCenterPosDummy(pDumPos);
        }
    }
    return true;
}

int CMapData::GetPortalInx(char *pPortalCode)
{
  for (int j = 0; j < m_nPortalNum; ++j)
  {
    if (!strcmp_0(pPortalCode, m_pPortal[j].m_pPortalRec->m_strCode))
    {
      return j;
    }
  }
  return -1;
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

_bsp_info *CMapData::GetBspInfo()
{
  return &this->m_BspInfo;
}

_sec_info *CMapData::GetSecInfo()
{
  return &this->m_SecInfo;
}

unsigned __int8 CMapData::GetMapCode()
{
  return static_cast<unsigned __int8>(this->m_pMapSet->m_dwIndex);
}

int CMapData::GetSectorIndex(float *pPos)
{
  const float localX = static_cast<float>(-this->m_BspInfo.m_nMapMinSize[0]) + pPos[0];
  const float localZ = static_cast<float>(this->m_BspInfo.m_nMapMaxSize[2]) - pPos[2];
  const unsigned int secX = static_cast<unsigned int>(static_cast<int>(localX / 100.0f));
  const unsigned int secY = static_cast<unsigned int>(static_cast<int>(localZ / 100.0f));

  if (secX < static_cast<unsigned int>(this->m_SecInfo.m_nSecNumW)
      && secY < static_cast<unsigned int>(this->m_SecInfo.m_nSecNumH))
  {
    return static_cast<int>(static_cast<unsigned int>(this->m_SecInfo.m_nSecNumW) * secY + secX);
  }

  return -1;
}

CObjectList *CMapData::GetSectorListObj(unsigned __int16 wLayerIndex, unsigned int dwSecIndex)
{
  return &this->m_ls[wLayerIndex].m_pListSectorObj[static_cast<unsigned __int64>(dwSecIndex)];
}

CObjectList *CMapData::GetSectorListPlayer(unsigned __int16 wLayerIndex, unsigned int dwSecIndex)
{
  return &this->m_ls[wLayerIndex].m_pListSectorPlayer[static_cast<unsigned __int64>(dwSecIndex)];
}

CObjectList *CMapData::GetSectorListTower(unsigned __int16 wLayerIndex, unsigned int dwSecIndex)
{
  return &this->m_ls[wLayerIndex].m_pListSectorTower[static_cast<unsigned __int64>(dwSecIndex)];
}

void CMapData::GetRectInRadius(_pnt_rect *pRect, int nRadius, unsigned int nSecNum)
{
  if (nSecNum < this->m_SecInfo.m_nSecNum)
  {
    int secX = static_cast<int>(nSecNum) % this->m_SecInfo.m_nSecNumW;
    int secY = static_cast<int>(nSecNum) / this->m_SecInfo.m_nSecNumW;

    pRect->nStartx = secX - nRadius;
    if (pRect->nStartx < 0)
      pRect->nStartx = 0;

    pRect->nStarty = secY - nRadius;
    if (pRect->nStarty < 0)
      pRect->nStarty = 0;

    pRect->nEndx = nRadius + secX;
    if (pRect->nEndx > this->m_SecInfo.m_nSecNumW - 1)
      pRect->nEndx = this->m_SecInfo.m_nSecNumW - 1;

    pRect->nEndy = nRadius + secY;
    if (pRect->nEndy > this->m_SecInfo.m_nSecNumH - 1)
      pRect->nEndy = this->m_SecInfo.m_nSecNumH - 1;
  }
  else
  {
    memset_0(pRect, 0, sizeof(_pnt_rect));
  }
}

void CMapData::EnterMap(CGameObject *pObj, unsigned int dwSecIndex)
{
  if (dwSecIndex >= this->m_SecInfo.m_nSecNum)
    return;

  _object_list_point *pItem = &pObj->m_SectorPoint;
  CObjectList *SectorListObj = this->GetSectorListObj(pObj->m_wMapLayerIndex, dwSecIndex);
  SectorListObj->PushItem(pItem);

  if (!pObj->m_ObjID.m_byKind)
  {
    if (pObj->m_ObjID.m_byID)
    {
      if (pObj->m_ObjID.m_byID == 1)
        ++this->m_nMapInMonsterNum;
    }
    else
    {
      ++this->m_nMapInPlayerNum;
    }
  }

  if (pObj->m_ObjID.m_byKind || pObj->m_ObjID.m_byID)
  {
    if (!pObj->m_ObjID.m_byKind && pObj->m_ObjID.m_byID == 4)
    {
      _object_list_point *pNetPoint = &pObj->m_SectorNetPoint;
      CObjectList *SectorListTower = this->GetSectorListTower(pObj->m_wMapLayerIndex, dwSecIndex);
      SectorListTower->PushItem(pNetPoint);
    }
  }
  else
  {
    _object_list_point *pNetPoint = &pObj->m_SectorNetPoint;
    CObjectList *SectorListPlayer = this->GetSectorListPlayer(pObj->m_wMapLayerIndex, dwSecIndex);
    SectorListPlayer->PushItem(pNetPoint);
  }
}

void CMapData::ExitMap(CGameObject *pObj, unsigned int dwSecIndex)
{
  if (dwSecIndex >= this->m_SecInfo.m_nSecNum)
    return;

  _object_list_point *pItem = &pObj->m_SectorPoint;
  CObjectList *SectorListObj = this->GetSectorListObj(pObj->m_wMapLayerIndex, dwSecIndex);
  SectorListObj->DeleteItem(pItem);

  if (!pObj->m_ObjID.m_byKind)
  {
    if (pObj->m_ObjID.m_byID)
    {
      if (pObj->m_ObjID.m_byID == 1)
        --this->m_nMapInMonsterNum;
    }
    else
    {
      --this->m_nMapInPlayerNum;
    }
  }

  if (pObj->m_ObjID.m_byKind || pObj->m_ObjID.m_byID)
  {
    if (!pObj->m_ObjID.m_byKind && pObj->m_ObjID.m_byID == 4)
    {
      _object_list_point *pNetPoint = &pObj->m_SectorNetPoint;
      CObjectList *SectorListTower = this->GetSectorListTower(pObj->m_wMapLayerIndex, dwSecIndex);
      SectorListTower->DeleteItem(pNetPoint);
    }
  }
  else
  {
    _object_list_point *pNetPoint = &pObj->m_SectorNetPoint;
    CObjectList *SectorListPlayer = this->GetSectorListPlayer(pObj->m_wMapLayerIndex, dwSecIndex);
    SectorListPlayer->DeleteItem(pNetPoint);
  }
}
