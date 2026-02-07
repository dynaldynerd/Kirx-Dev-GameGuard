#pragma once

#include "IdaCompat.h"
#include "CTSingleton.h"

class CashDbWorker;

class __cppobj CCashDBWorkManager : public CTSingleton<CCashDBWorkManager>
{
public:
  bool Initialize();

  CashDbWorker *m_pWorker;
};
