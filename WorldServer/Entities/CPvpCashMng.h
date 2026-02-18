#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _class_fld;

class  CPvpCashMng
{
public:
  CPvpCashMng();
  ~CPvpCashMng();

  struct __declspec(align(8)) _talik_recvr
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

  struct __declspec(align(8)) _class_value
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
  bool LoadData();
  char IsTalikItem(const char *strCode);
  int GetTalikRecvrPoint(unsigned __int8 byTblCode, unsigned int dwIndex);
  int GetTalikRecvrPoint(int i);

private:
  bool Parsing(const char *szTitle, const char *szItem, char **szBuff, int nInx, unsigned __int8 byParseType);
  bool SetItem(char *szItemCode, int nInx);
};
