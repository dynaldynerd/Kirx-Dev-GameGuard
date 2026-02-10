#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

class AutominePersonal;
struct _PERSONALAMINE_INVEN_DB_BASE;

class __cppobj AutominePersonalMgr
{
public:
  static AutominePersonalMgr *instance();
  bool initialize();
  AutominePersonal *get_machine(unsigned int nIdx);
  char db_load_inven(unsigned int dwSerial, _PERSONALAMINE_INVEN_DB_BASE *pCon);
  unsigned __int8 request_query(char *pdata);
  void result_query(unsigned __int8 byRet, char *pdata);
  void pop_dqs_makestorage(unsigned __int8 byRet, char *pdata);
  void send_ecode(unsigned int n, unsigned __int8 byCode);

private:
  bool init_objects();

  CLogFile m_logService;
  CLogFile m_logError;
  unsigned int m_dwObjSerial;
  AutominePersonal *m_pMachine;
  unsigned __int16 m_wOreIndex[5];
};
