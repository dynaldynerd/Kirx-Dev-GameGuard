#pragma once

#include "IdaCompat.h"
#include "CTSingleton.h"

class __cppobj CCashDBWorkManager : public CTSingleton<CCashDBWorkManager>
{
public:
  bool Initialize();
};
