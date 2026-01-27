#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

class AutominePersonal;

class __cppobj AutominePersonalMgr
{
public:
  static AutominePersonalMgr *instance();
  bool initialize();

private:
  bool init_objects();

  CLogFile m_logService;
  CLogFile m_logError;
  unsigned int m_dwObjSerial;
  AutominePersonal *m_pMachine;
  unsigned __int16 m_wOreIndex[5];
};
