#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "StorageList.h"

class AutominePersonal;
class CMapData;
class CPlayer;
struct _PERSONALAMINE_INVEN_DB_BASE;
struct _personal_amine_infoui_open_clzo;
struct _personal_amine_make_storage_clzo;
struct _personal_automine_battery_extract_clzo;
struct _personal_automine_battery_insert_clzo;
struct _personal_automine_install_clzo;
struct _personal_automine_popore_clzo;
struct _personal_automine_selore_clzo;
struct _personal_automine_uninstall_clzo;
struct _qry_case_make_storage;

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
  bool db_load_inven(unsigned int dwSerial, _PERSONALAMINE_INVEN_DB_BASE *pCon);
  bool make_storagebox(int n, const _personal_amine_make_storage_clzo *request);
  bool install(int n, const _personal_automine_install_clzo *request);
  bool uninstall(int n, const _personal_automine_uninstall_clzo *request);
  bool uninstall(int n);
  bool selectore(unsigned int n, const _personal_automine_selore_clzo *request);
  bool insert_battery(unsigned int n, const _personal_automine_battery_insert_clzo *request);
  bool extract_battery(unsigned int n, const _personal_automine_battery_extract_clzo *request);
  bool extract_battery(unsigned int n);
  bool pop_ore(unsigned int n, const _personal_automine_popore_clzo *request);
  bool Open_InvenUI(int n, const _personal_amine_infoui_open_clzo *request);
  bool Open_InfoUI(unsigned int n, const _personal_amine_infoui_open_clzo *request);
  bool Is_MineRun(int n);
  int request_query(const _qry_case_make_storage *query);
  void result_query(unsigned __int8 byRet, const _qry_case_make_storage *query);
  void pop_dqs_makestorage(unsigned __int8 byRet, const _qry_case_make_storage *query);
  void push_dqs_makestorage(int n, unsigned int dwAvatorSerial, unsigned int dwTotGold);
  void send_ecode(unsigned int n, unsigned __int8 byCode);

private:
  bool init_objects();
  bool check_dummy(CMapData *pMap, unsigned __int8 byCurDummyIndex, float *pfCurPos);
  bool check_machine(int n, unsigned int dwSerial, AutominePersonal *pMachine, _STORAGE_LIST::_db_con *pcitem);
  _STORAGE_LIST::_db_con *get_localitem(CPlayer *pOne, unsigned __int16 wItemSerial);

  CLogFile m_logService;
  CLogFile m_logError;
  unsigned int m_dwObjSerial;
  AutominePersonal *m_pMachine;
  unsigned __int16 m_wOreIndex[5];
};
