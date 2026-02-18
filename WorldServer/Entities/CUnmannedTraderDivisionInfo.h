#pragma once

#include "IdaCompat.h"
#include <vector>

class CLogFile;
class TiXmlElement;
class CUnmannedTraderClassInfo;
class CUnmannedTraderSortType;

class  CUnmannedTraderDivisionInfo
{
public:
  CUnmannedTraderDivisionInfo(unsigned int dwID, char *szName);
  ~CUnmannedTraderDivisionInfo();

  void CleanUp();
  bool LoadXML(TiXmlElement *pkElement, CLogFile *kLogger);
  unsigned int GetID();
  unsigned __int64 GetMaxClassCnt();
  unsigned long long GetSize();
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
  bool IsExistSortTypeID(unsigned int dwID);
  bool IsValidID(unsigned int dwID);
  char IsExistGroupID(
    unsigned __int8 byDivision,
    unsigned __int8 byClass,
    unsigned __int8 bySubClass,
    unsigned __int8 bySortType,
    unsigned int *dwListIndex);
  CUnmannedTraderSortType *GetSortType(unsigned __int8 bySortType);

  unsigned int m_dwID;
  char m_szName[128];
  std::vector<CUnmannedTraderClassInfo *> m_vecClass;
  std::vector<CUnmannedTraderSortType *> m_vecSortType;
};
