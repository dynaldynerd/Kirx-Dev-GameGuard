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
struct _buy_offer;
struct _sell_offer;

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
  void reward_add_money(
    int n,
    const char *pszClause,
    unsigned int dwAddDalant,
    unsigned int dwAddGold,
    unsigned int dwSumDalant,
    unsigned int dwSumGold,
    char *pszFileName);
  void reward_add_item(int n, const char *pszClause, _STORAGE_LIST::_db_con *pItem, char *pszFileName);
  void exchange_money(
    int n,
    unsigned int dwCurDalant,
    unsigned int dwCurGold,
    unsigned int dwNewDalant,
    unsigned int dwNewGold,
    char *pszFileName);
  void buy_item(
    int n,
    _buy_offer *pOffer,
    unsigned __int8 byOfferNum,
    unsigned int dwCostDalant,
    unsigned int dwCostGold,
    unsigned int dwNewDalant,
    unsigned int dwNewGold,
    char *pszFileName);
  void sell_item(
    int n,
    _sell_offer *pOffer,
    unsigned __int8 byOfferNum,
    unsigned int dwCostDalant,
    unsigned int dwCostGold,
    unsigned int dwNewDalant,
    unsigned int dwNewGold,
    char *pszFileName);
  void add_storage_fail(
    int n,
    _STORAGE_LIST::_db_con *pItem,
    const char *strErrorCodePos,
    char *pszFileName);
  void make_item(
    int n,
    _STORAGE_LIST::_db_con *pMaterial,
    unsigned __int8 *pbyMtrNum,
    unsigned __int8 byMaterialNum,
    unsigned __int8 byRetCode,
    bool bInsert,
    _STORAGE_LIST::_db_con *pMakeItem,
    char *pszFileName);
  void cheat_make_item_no_material(
    int n,
    unsigned __int8 byRetCode,
    _STORAGE_LIST::_db_con *pMakeItem,
    char *pszFileName);
  void grade_up_item(
    int n,
    _STORAGE_LIST::_db_con *pItem,
    _STORAGE_LIST::_db_con *pTalik,
    _STORAGE_LIST::_db_con *pJewel,
    unsigned __int8 byJewelNum,
    unsigned __int8 byErrCode,
    unsigned int dwAfterLv,
    char *pszFileName);
  void grade_down_item(
    int n,
    _STORAGE_LIST::_db_con *pItem,
    _STORAGE_LIST::_db_con *pTalik,
    unsigned int dwAfterLv,
    char *pszFileName);
  void combine_item(
    int n,
    _STORAGE_LIST::_db_con *pMaterial,
    unsigned __int8 *pbyMtrNum,
    unsigned __int8 byMaterialNum,
    _STORAGE_LIST::_db_con *pMakeItem,
    unsigned int dwFee,
    unsigned int dwLeftDalant,
    char *pszFileName);
  void exchange_item(
    int n,
    _STORAGE_LIST::_db_con *pUseItem,
    _STORAGE_LIST::_db_con *pOutItem,
    char *pszFileName);
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
  void InitClass(
    int iCostGold,
    unsigned int dwInitClassCnt,
    unsigned __int8 byLastClassGrade,
    char *szOldClass,
    char *szCurClass,
    int *piOldMaxPoint,
    int *piAlterMaxPoint,
    char *pszFileName);
  void ClassUP(
    unsigned __int8 byCurClassGrade,
    unsigned __int8 byLastClassGrade,
    char *szOldClass,
    char *szCurClass,
    int *piOldMaxPoint,
    int *piAlterMaxPoint,
    char *pszFileName);
  void mastery_change_jade(
    int nMstCode,
    unsigned int dwOldCum,
    unsigned int dwNewCum,
    int nLv,
    float fVal,
    char *szFileName,
    int nWpType);
  void GetNewFileName(unsigned int dwAvatorSerial, char *pszFileName);
  void have_auto_item(int n, const CUnmannedTraderRegistItemInfo *pkInfo, unsigned __int8 byMaxCnt);
  void have_item(
    int n,
    char *pszName,
    _AVATOR_DATA *pLoadData,
    _AVATOR_DATA *pBackupData,
    char *pszID,
    unsigned int dwIDSerial,
    unsigned __int8 byDgr,
    unsigned int dwIP,
    unsigned int dwExpRate,
    bool bStart,
    char *pszFileName);
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
  void throw_ground_item(
    int n,
    _STORAGE_LIST::_db_con *pItem,
    char *pMapCode,
    float *pfPos,
    char *pszFileName);
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
  void guild_pop_money(
    int n,
    char *pszGuildName,
    unsigned int dwPopDalant,
    unsigned int dwPopGold,
    unsigned int dwLeftDalant,
    unsigned int dwLeftGold,
    char *pszFileName);
  void guild_pop_money_rollback(
    int n,
    char *pszGuildName,
    unsigned int dwPopDalant,
    unsigned int dwPopGold,
    unsigned int dwLeftDalant,
    unsigned int dwLeftGold,
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
