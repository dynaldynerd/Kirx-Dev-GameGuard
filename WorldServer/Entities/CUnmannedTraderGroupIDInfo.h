#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include <vector>

class CUnmannedTraderDivisionInfo;
class CUnmannedTraderSortType;

class  CUnmannedTraderGroupIDInfo
{
public:
  CUnmannedTraderGroupIDInfo();
  ~CUnmannedTraderGroupIDInfo();

  void CleanUp();
  void SetLogger(CLogFile *pkLogger);
  void Log(const char *fmt, ...);
  bool IsExistID(unsigned int dwID);
  bool GetIDInfo(std::vector<std::pair<unsigned long, unsigned long>> *vecInfo);
  bool GetGroupID(
    unsigned __int8 byTableCode,
    unsigned __int16 wItemTableIndex,
    unsigned __int8 *byDivision,
    unsigned __int8 *byClass);
  bool GetGroupID(
    unsigned __int8 byTableCode,
    unsigned __int16 wItemTableIndex,
    unsigned __int8 *byDivision,
    unsigned __int8 *byClass,
    unsigned __int8 *bySubClass,
    unsigned int *dwListIndex);
  bool IsExistGroupID(
    unsigned __int8 byDivision,
    unsigned __int8 byClass,
    unsigned __int8 bySubClass,
    unsigned __int8 bySortType,
    unsigned int *dwListIndex);
  CUnmannedTraderSortType *GetSortType(unsigned __int8 byDivision, unsigned __int8 bySortType);
  bool LoadXML(const char *szFileName);

  CLogFile *m_pkLogger;
  std::vector<CUnmannedTraderDivisionInfo *> m_vecDivisionInfo;
};
