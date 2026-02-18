#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _ItemMakeData_fld : _base_fld
{
  struct _material_list
  {
    char m_itmPdMat[8];
    int m_nPdMatNum;
  };

  struct _output_list
  {
    char m_itmPdOutput[8];
    unsigned int m_dwPdProp;
  };

  int m_nMakeMastery;
  char m_strCivil[64];
  _material_list m_listMaterial[5];
  _output_list m_listOutput[30];
};
#pragma pack(pop)
