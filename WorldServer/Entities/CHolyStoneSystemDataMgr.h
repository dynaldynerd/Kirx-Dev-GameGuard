#pragma once

#include "IdaCompat.h"

class CHolyStoneSystem;
class CHolyScheduleData;
class CHolyStoneSaveData;

class __cppobj CHolyStoneSystemDataMgr
{
public:
  static bool LoadIni(CHolyStoneSystem *clsHolyStoneSystem);
  static bool LoadSceduleData(CHolyScheduleData *clsDummy);
  static bool LoadStateData(CHolyStoneSaveData *clsSaveDummy);
  static bool SaveStateData(CHolyStoneSaveData *clsSaveDummy);
};
