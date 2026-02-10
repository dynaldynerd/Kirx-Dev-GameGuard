#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"
#include "StorageList.h"

class CPostStorage;
class CPostReturnStorage;
class CPostData;
class CUnmannedTraderRegistItemInfo;
struct _AVATOR_DATA;

class __cppobj CMgrAvatorItemHistory
{
public:
  struct __LOG_DATA
  {
    char szFileName[64];
    int nLen;
  };

  struct __cppobj __LOG_DATA_10K : public __LOG_DATA
  {
    char sData[10000];
  };

  struct __cppobj __LOG_DATA_1K : public __LOG_DATA
  {
    char sData[1000];
  };

  struct __cppobj __LOG_DATA_200 : public __LOG_DATA
  {
    char sData[200];
  };

  CMgrAvatorItemHistory();
  static void IOThread(void *pv);
  void WriteFile(const char *pszFileName, const char *pszLog);
  void char_copy(int n, char *pszDstName, unsigned int dwDstSerial, char *pszFileName);
  void cheat_alter_money(int n, unsigned int dwNewDalant, unsigned int dwNewGold, char *pszFileName);
  void cheat_add_item(int n, _STORAGE_LIST::_db_con *pItem, unsigned __int8 byAddNum, char *pszFileName);
  void cheat_del_item(int n, _STORAGE_LIST::_db_con *pItem, unsigned __int8 byDelNum, char *pszFileName);
  void delete_npc_quest_item(int n, _STORAGE_LIST::_db_con *pItem, char *pszFileName);
  void consume_del_item(int n, _STORAGE_LIST::_db_con *pItem, const char *pszFileName);
  void reward_add_item(int n, char *pszClause, _STORAGE_LIST::_db_con *pItem, char *pszFileName);
  void personal_amine_itemlog(
    const char *szLogDesc,
    unsigned __int8 byPos,
    unsigned __int8 byTblCode,
    unsigned __int16 wItemIndex,
    unsigned int dwDur,
    const char *szFileName);
  void personal_amine_uninstall(
    unsigned __int8 byType,
    const unsigned int *pdwMineCnt,
    int nMaxOreNum,
    _STORAGE_LIST::_db_con *pItem,
    const char *szFileName);
  void lenditem_del_from_inven(
    unsigned __int8 byTblCode,
    unsigned __int16 wItemIndex,
    unsigned __int64 lnUID,
    char *pFN);
  void time_jade_effect_log(
    char *pszItemName,
    _STORAGE_LIST::_db_con *pItem,
    bool bAdd,
    char *pszFileName);
  void exp_prof_log(int count, char *szFile);
  void mastery_change_jade(
    int nMstCode,
    unsigned int dwOldCum,
    unsigned int dwNewCum,
    int nLv,
    float fVal,
    char *szFileName,
    int nWpType);
  void have_auto_item(int n, const CUnmannedTraderRegistItemInfo *pkInfo, unsigned __int8 byMaxCnt);
  void have_item_close(
    int n,
    char *pszName,
    _AVATOR_DATA *pLoadData,
    _AVATOR_DATA *pBackupData,
    char *pszID,
    unsigned int dwIDSerial,
    unsigned __int8 byDgr,
    unsigned int dwIP,
    unsigned int dwExpRate,
    const CUnmannedTraderRegistItemInfo *pkInfo,
    unsigned __int8 byMaxCnt,
    char *pszFileName);
  void reg_auto_trade(
    int n,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pRegItem,
    unsigned int dwPrice,
    unsigned int dwfee,
    unsigned int dwLeftDalant,
    char *pszFileName);
  void self_cancel_auto_trade(
    int n,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pRegItem,
    char *pszFileName);
  void time_out_cancel_auto_trade(
    int n,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pRegItem,
    char *pszFileName);
  void price_auto_trade(
    int n,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pRegItem,
    unsigned int dwTax,
    unsigned int dwOldPrice,
    unsigned int dwNewPrice,
    char *pszFileName);
  void re_reg_auto_trade(
    int n,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pRegItem,
    unsigned int dwPrice,
    unsigned int dwfee,
    unsigned int dwLeftDalant,
    char *pszFileName);
  void auto_trade_sell(
    const char *szBuyerName,
    unsigned int dwBuyerSerial,
    const char *szBuyerID,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pItem,
    unsigned int dwPrice,
    unsigned int dwTax,
    unsigned int dwLeftDalant,
    unsigned int dwLeftGold,
    char *pszFileName);
  void auto_trade_buy(
    const char *szSellerName,
    unsigned int dwSellerSerial,
    const char *szSellerID,
    unsigned int dwRegistSerial,
    _STORAGE_LIST::_db_con *pItem,
    unsigned int dwPrice,
    unsigned int dwLeftDalant,
    unsigned int dwLeftGold,
    char *pszFileName);
  void take_ground_item(
    int n,
    unsigned __int8 byItemBoxCode,
    _STORAGE_LIST::_db_con *pItem,
    const char *pszThrowerName,
    unsigned int dwThrowerSerial,
    const char *pszThrowerID,
    unsigned __int16 wMonRecIndex,
    const char *pMapCode,
    float *pfPos,
    const char *pszFileName);
  void trans_ground_item(
    _STORAGE_LIST::_db_con *pItem,
    const char *pszTakerName,
    unsigned int dwTakerSerial,
    const char *pszTakerID,
    const char *pszFileName);
  void post_receive(CPostData *pPost, char *pFileName);
  void post_storage(CPostStorage *pStorage, char *pFileName);
  void post_returnreceive(CPostData *pPost, char *pFileName);
  void return_post_storage(CPostReturnStorage *pReturn, char *pFileName);
  void patriarch_push_money(
    char *pwszPatriarchName,
    unsigned int dwPushDalant,
    unsigned int dwLeftDalant,
    char *pszFileName);
  void raceboss_giveback(unsigned int dwSerial, unsigned int dwDalant, char *pszFileName);
  void guild_est_money_rollback(
    int n,
    char *pszGuildName,
    unsigned int dwEstDalant,
    unsigned int dwLeftDalant,
    char *pszFileName);
  void close(int n, char *pCloseCode, char *pszFileName);
  void read_cashamount(unsigned int dwAC, unsigned int dwAV, int nCash, char *pFileName);
  void buy_to_inven_cashitem(
    unsigned __int8 byTbl,
    unsigned __int16 wIndex,
    int nPrice,
    int nDis,
    int nNum,
    int nBuyPrice,
    int nAmount,
    char *pFileName,
    unsigned __int64 lnUID,
    unsigned __int8 byEventType);
  void cash_item_use(int n, _STORAGE_LIST::_db_con *pUseItem, char *pszFileName);
  void cashitem_del_from_inven(unsigned __int8 byTblCode, unsigned __int16 wItemIndex, unsigned __int64 lnUID, char *pFN);
  void rollback_cashitem(char *szRet, unsigned __int64 lnUID, char *strItemCode, int nCash, char *pFileName);
  void used_cash(int nCurCash, int nUseCash, char *pFileName);

  char m_szStdPath[128];
  unsigned int m_dwLastLocalDate;
  unsigned int m_dwLastLocalHour;
  CMyTimer m_tmrUpdateTime;
  char m_szCurDate[32];
  char m_szCurTime[32];
  __LOG_DATA_10K m_LogData_10K[254];
  CNetIndexList m_listLogData_10K;
  CNetIndexList m_listLogDataEmpty_10K;
  __LOG_DATA_1K m_LogData_1K[254];
  CNetIndexList m_listLogData_1K;
  CNetIndexList m_listLogDataEmpty_1K;
  __LOG_DATA_200 m_LogData_200[2532];
  CNetIndexList m_listLogData_200;
  CNetIndexList m_listLogDataEmpty_200;
  bool m_bIOThread;
  CFrameRate m_FrameRate;
};
