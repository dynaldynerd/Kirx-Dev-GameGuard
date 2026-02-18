#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _MONEY_SUPPLY_DATA
{
  struct _money_supply
  {
    int nLv[4];
    int nRace[3];
    int nClass[60];
  };

  __int64 dwAmount[9];
  _money_supply ms_data[4];
  int nFeeLv[4];
  int nFeeRace[3];
  int nHonorGuildRace[2][3];
  int nUnitRepairLv[4];
  int nBuyUnitLv[4];

  void init();
};
#pragma pack(pop)

class  CMoneySupplyMgr
{
public:
  static CMoneySupplyMgr *Instance();
  void Initialize();
  void LoopMoneySupply();
  void SendMsg_MoneySupplyDataToWeb(_MONEY_SUPPLY_DATA *pMSData);
  void UpdateBuyData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount);
  void UpdateSellData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount);
  void UpdateQuestRewardMoneyData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount);
  void UpdateBuyUnitData(int nLv, unsigned int nAmount);
  void UpdateUnitRepairingChargesData(int nLv, unsigned int nAmount);
  void UpdateFeeMoneyData(unsigned __int8 byRace, int nLv, unsigned int nAmount);
  void UpdateHonorGuildMoneyData(unsigned __int8 byTradeType, unsigned __int8 byRace, unsigned int nAmount);
  CMoneySupplyMgr();
  virtual ~CMoneySupplyMgr() = default;

  static CMoneySupplyMgr *pInstance;

  unsigned int m_dwLastSendTime;
  unsigned int m_dwSystemOperStartTime;
  _MONEY_SUPPLY_DATA m_MS_data;
  _MONEY_SUPPLY_DATA m_MS_Senddata;
};
