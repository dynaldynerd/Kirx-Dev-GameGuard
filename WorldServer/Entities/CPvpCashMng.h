#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _class_fld;

class __cppobj CPvpCashMng
{
public:
  struct __declspec(align(8)) _talik_recvr
  {
    unsigned __int8 byTableCode;
    _base_fld *m_pFld;
    int nRecvrPoint;
  };

  struct __cppobj _talik_recvr_list
  {
    unsigned __int8 byTalikNum;
    _talik_recvr TalikInfo[14];
  };

  struct __declspec(align(8)) _class_value
  {
    _class_fld *m_pFld;
    unsigned __int8 byClassVal;
  };

  struct __cppobj _max_point
  {
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
};
