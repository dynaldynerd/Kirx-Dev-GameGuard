#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "StorageList.h"

class AutominePersonal;
class CMapData;
class CPlayer;
struct _PERSONALAMINE_INVEN_DB_BASE;

class  AutominePersonalMgr
{
public:
  static AutominePersonalMgr *m_pInstance;
  static AutominePersonalMgr *instance();
  static void release();
  AutominePersonalMgr();
  bool initialize();
  bool CreateDBTable();
  AutominePersonal *get_machine(unsigned int nIdx);
  char db_load_inven(unsigned int dwSerial, _PERSONALAMINE_INVEN_DB_BASE *pCon);
  char make_storagebox(int n, char *pmsg);
  char install(int n, char *pmsg);
  char uninstall(int n, char *pmsg);
  char uninstall(int n);
  char selectore(unsigned int n, char *pmsg);
  char insert_battery(unsigned int n, char *pmsg);
  char extract_battery(unsigned int n, char *pmsg);
  char extract_battery(unsigned int n);
  char pop_ore(unsigned int n, char *pmsg);
  char Open_InvenUI(int n, bool *pmsg);
  char Open_InfoUI(unsigned int n, bool *pmsg);
  bool Is_MineRun(int n);
  unsigned __int8 request_query(char *pdata);
  void result_query(unsigned __int8 byRet, char *pdata);
  void pop_dqs_makestorage(unsigned __int8 byRet, char *pdata);
  void push_dqs_makestorage(int n, unsigned int dwAvatorSerial, unsigned int dwTotGold);
  void send_ecode(unsigned int n, unsigned __int8 byCode);

private:
  bool init_objects();
  bool check_dummy(CMapData *pMap, unsigned __int8 byCurDummyIndex, float *pfCurPos);
  char check_machine(int n, unsigned int dwSerial, AutominePersonal *pMachine, _STORAGE_LIST::_db_con *pcitem);
  _STORAGE_LIST::_db_con *get_localitem(CPlayer *pOne, unsigned __int16 wItemSerial);

  CLogFile m_logService;
  CLogFile m_logError;
  unsigned int m_dwObjSerial;
  AutominePersonal *m_pMachine;
  unsigned __int16 m_wOreIndex[5];
};
