#pragma once

#include "IdaCompat.h"

#include "CTSingleton.h"

class CBilling;

class CNationSettingData;
struct _NameTxt_fld;

class __cppobj CNationSettingManager : public CTSingleton<CNationSettingManager>
{
public:
  int Init(int nationCode, const char *nationCodeStr, bool serviceMode);
  CBilling *CreateBilling();
  const char *GetItemName(_NameTxt_fld *pFld);

  CNationSettingData *m_pData;
};
