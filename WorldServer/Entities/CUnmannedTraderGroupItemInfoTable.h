#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderGroupIDInfo.h"
#include "CUnmannedTraderGroupVersionInfo.h"

class  CUnmannedTraderGroupItemInfoTable
{
public:
  static CUnmannedTraderGroupItemInfoTable *Instance();
  static void Destroy();

  CUnmannedTraderGroupItemInfoTable();
  ~CUnmannedTraderGroupItemInfoTable();
  bool Init();
  void SetLogger(CLogFile *pkLogger);
  void Log(const char *fmt, ...);
  bool GetGroupID(
    unsigned __int8 byTableCode,
    unsigned __int16 wItemTableIndex,
    unsigned __int8 *byDivision,
    unsigned __int8 *byClass,
    unsigned __int8 *bySubClass,
    unsigned int *dwListIndex);
  bool GetVersion(unsigned __int8 byDivision, unsigned __int8 byClass, unsigned int *dwVer);
  bool IsExistGroupID(
    unsigned __int8 byDivision,
    unsigned __int8 byClass,
    unsigned __int8 bySubClass,
    unsigned __int8 bySortType,
    unsigned int *dwListIndex);
  CUnmannedTraderSortType *GetSortType(unsigned __int8 byDivision, unsigned __int8 bySortType);
  char IncreaseVersion(unsigned __int8 byTableCode, unsigned __int16 wItemTableIndex);
  char IncreaseVersion(
    unsigned __int8 byTableCode,
    unsigned __int16 wItemTableIndex,
    unsigned __int8 byRegistDivision,
    unsigned __int8 byRegistClass);
  char IncreaseVersion(unsigned __int8 byDivision, unsigned __int8 byClass);

  static CUnmannedTraderGroupItemInfoTable *ms_Instance;

  CLogFile *m_pkLogger;
  CUnmannedTraderGroupVersionInfo m_kVerInfo;
  CUnmannedTraderGroupIDInfo m_kGroupIDInfo;
};
