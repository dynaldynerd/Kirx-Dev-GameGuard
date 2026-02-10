#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "CUnmannedTraderRequestLimiter.h"
#include <vector>

class CPlayer;
class CLogFile;

class __cppobj CUnmannedTraderUserInfo
{
public:
  enum class LOG_IN_STATE : int
  {
    UTUI_EMPTY = 0,
    UTUI_LOGIN = 1,
    UTUI_NONE = -1,
  };

  CUnmannedTraderUserInfo();

  bool Init(unsigned short wInx);
  void Clear();
  void ClearLoadItemInfo();
  void ClearRequest();
  std::vector<CUnmannedTraderRegistItemInfo>::iterator Find(unsigned int dwRegistSerial);
  std::vector<CUnmannedTraderRegistItemInfo>::iterator FindEmpty();
  void CountRegistItem();
  void GetCurrentRegItemStateStr(char *szStateStr, int iBuffSize);
  CUnmannedTraderItemState::STATE GetCloseItemForPassTimeUpdateInfo(
    unsigned int dwRegistSerial,
    CPlayer **pkOwner);
  bool CompleteUpdateState(unsigned int dwRegistSerial, unsigned __int8 byState, bool bReCountRegist);
  void CompleteRegist(unsigned __int8 byRet, char *pLoadData, CLogFile *pkLogger);
  bool CompleteRegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 dwItemSerial,
    unsigned int dwETSerialNumber,
    unsigned int dwPrice,
    unsigned __int8 bySellTurm,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex,
    unsigned __int8 byStorageIndex,
    unsigned __int64 dwD,
    unsigned int dwU,
    bool bInserted);
  void CompleteCancelRegist(unsigned __int8 byRet, char *pLoadData, CLogFile *pkLogger);
  bool CompleteCancelRegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 dwItemSerial,
    CLogFile *pkLogger);
  void CompleteTimeOutClear(unsigned int dwRegistSerial, CLogFile *pkLogger);
  void CompleteReprice(unsigned __int8 byRet, char *pLoadData, CLogFile *pkLogger);
  bool CompleteRepriceItem(unsigned int dwRegistSerial, unsigned __int16 dwItemSerial, unsigned int dwPrice);
  void CompleteReRegist(char *pLoadData, CLogFile *pkLogger);
  bool CompleteReRegistItem(
    unsigned int dwRegistSerial,
    unsigned __int16 dwItemSerial,
    unsigned int dwPrice,
    CLogFile *pkLogger,
    unsigned __int8 *pbyProcRet);
  void CompleteUpdateCheatRegistTime(char *pLoadData);
  void CompleteReRegistRollBack(char *pData, CLogFile *pkLogger);
  unsigned __int8 CheckSellComplete(
    CPlayer *pkSellPlayer,
    CPlayer *pkBuyer,
    unsigned int dwRegistSerial,
    unsigned int dwRealPrice,
    CLogFile *pkLogger);
  unsigned __int8 SellComplete(
    CPlayer *pkSellPlayer,
    CPlayer *pkBuyer,
    unsigned int dwOriPrice,
    unsigned int dwRealPrice,
    unsigned int dwTax,
    unsigned int dwRegistSerial,
    __int64 tResultTime,
    CLogFile *pkLogger);
  unsigned __int8 CheckBuyComplete(CPlayer *pkBuyer, unsigned int dwPrice);
  unsigned __int8 BuyComplete(
    CPlayer *pkBuyer,
    unsigned int dwSellerSerial,
    const char *wszSellerName,
    const char *szSellerAccountName,
    unsigned int dwRegistSerial,
    unsigned int dwK,
    unsigned __int64 dwD,
    unsigned int dwU,
    unsigned int dwPrice,
    unsigned __int64 lnUID,
    CLogFile *pkLogger,
    unsigned __int16 *wAddItemSerial);
  void NotifyRegistItem();
  void SendRegistItemSuccessResult(unsigned int dwLeftDalant, unsigned __int16 wInx, char *pLoadData);
  void SendRegistItemErrorResult(
    unsigned __int16 wInx,
    char byRet,
    unsigned __int16 wItemSerial,
    unsigned int dwRetParam1);
  void SendCancelRegistSuccessResult(unsigned __int16 wInx, unsigned __int16 wItemSerial, unsigned int dwRegistSerial);
  void SendCancelRegistErrorResult(unsigned __int16 wInx, unsigned __int8 byRet);
  void SendRepriceSuccessResult(
    CPlayer *pReceiver,
    unsigned __int16 wItemSerial,
    unsigned int dwNewPrice,
    unsigned int dwRegistSerial,
    unsigned int dwTax);
  void SendRepriceErrorResult(CPlayer *pReceiver, unsigned __int8 byRet);
  void SendSellInfom(
    unsigned __int16 wInx,
    unsigned __int16 wItemSerial,
    unsigned int dwAddDalant,
    unsigned int dwTaxDalant,
    unsigned int dwTotalDalant);
  void SendNotifyCloseItem(
    unsigned __int16 wInx,
    unsigned __int16 wItemSerial,
    unsigned int dwRegistSerial,
    unsigned int dwPrice,
    unsigned __int8 byTaxRate);
  CPlayer *FindOwner();
  unsigned __int16 GetIndex();
  bool IsLogInState();
  unsigned int GetSerial();
  unsigned __int8 GetMaxRegistCnt();
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo();
  bool IsNull();
  void SendSearchErrorResult(unsigned __int16 wInx, char byRet);
  void SendSearchResult(unsigned __int16 wInx, char *pLoadData);
  void LogOut(unsigned int dwSerial, CLogFile *pkLogger);

  bool operator==(unsigned int dwSerial);
  CUnmannedTraderUserInfo &operator=(const CUnmannedTraderUserInfo &rhs);

  static CUnmannedTraderUserInfo ms_kNull;

  LOG_IN_STATE m_eState;
  unsigned short m_wInx;
  unsigned int m_dwUserSerial;
  unsigned char m_byRegistCnt;
  unsigned char m_byMaxRegistCnt;
  CUnmannedTraderRequestLimiter m_kRequestState;
  std::vector<CUnmannedTraderRegistItemInfo> m_vecRegistItemInfo;
  std::vector<CUnmannedTraderRegistItemInfo> m_vecLoadItemInfo;
};
