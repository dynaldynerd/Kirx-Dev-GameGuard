#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _class_fld;

class  CPvpCashMng
{
public:
  CPvpCashMng();
  ~CPvpCashMng();
  static CPvpCashMng *_pkInstance;

  struct _talik_recvr
  {
    unsigned __int8 byTableCode;
    _base_fld *m_pFld;
    int nRecvrPoint;
  };

  struct  _talik_recvr_list
  {
    _talik_recvr_list();

    unsigned __int8 byTalikNum;
    _talik_recvr TalikInfo[14];
  };

  struct _class_value
  {
    _class_value();
    static void init(_class_value *self);

    _class_fld *m_pFld;
    unsigned __int8 byClassVal;
  };

  struct  _max_point
  {
    _max_point();
    static void init(_max_point *self);

    int nLv;
    int nMaxPoint;
    int nMinPoint;
    int nPremiumMaxPoint;
  };

  _talik_recvr_list m_TalikList;
  _class_value m_ClassValList[50];
  _max_point m_LimitPoint[30];

  static CPvpCashMng *Instance();
  void Release();
  bool LoadData();
  char IsTalikItem(const char *strCode);
  unsigned __int8 GetTalikNum();
  int GetTalikRecvrPoint(unsigned __int8 byTblCode, unsigned int dwIndex);
  int GetTalikRecvrPoint(int i);
  int GetMyClassVal(char *pClass);
  int GetMaxTempPoint(unsigned __int8 nLv, bool bPremium);
  int GetMinTempPoint(unsigned __int8 nLv);

private:
  bool Parsing(const char *szTitle, const char *szItem, char **szBuff, int nInx, unsigned __int8 byParseType);
  bool SetItem(char *szItemCode, int nInx);
};

