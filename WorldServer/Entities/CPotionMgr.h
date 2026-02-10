#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"
#include "PotionItem_fld.h"

class CRFWorldDatabase;

class __cppobj CPotionMgr
{
public:
  bool DatafileInit();
  bool SetPotionDataName();
  bool InsertRenamePotion(CRFWorldDatabase *pkWorldDB, char *pData);
  bool IsPotionDelayUseIndex(unsigned int nIndex);
  void Complete_RenameChar_DB_Select(unsigned __int8 byRet, char *p);
  void Complete_RenameChar_DB_Update(unsigned __int8 byRet, char *p);
  void PushRenamePotionDBLog(char *pInfo);

  struct __cppobj PotionInnerData
  {
    bool m_bPotionDelayIndexList[38];
  };

  struct __declspec(align(8)) __cppobj _ParamData
  {
    float fValue;
    _PotionItem_fld *pPotionFld;
    unsigned int dwCurTime;
    unsigned int dwEndTime;
    unsigned int dwNextTime;
  };

  PotionInnerData m_PotionInnerData;
  CRecordData m_tblPotionEffectData;
  CRecordData m_tblPotionCheckData;
};

extern CPotionMgr g_PotionMgr;
