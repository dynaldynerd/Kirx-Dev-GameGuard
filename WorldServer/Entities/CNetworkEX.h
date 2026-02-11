#pragma once

#include "IdaCompat.h"

#include "CNetProcess.h"

struct _CLID;

class __cppobj __declspec(align(8)) CNetWorking
{
public:
  CNetWorking();
  virtual ~CNetWorking();

  _socket *GetSocket(unsigned int dwProID, unsigned int dwSocketIndex);
  void CloseSocket(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose);
  bool SetNetSystem(unsigned int dwUseProcessNum, _NET_TYPE_PARAM *pType, char *szSystemName, char *pszLogPath);
  void Release();
  int Connect(unsigned int dwProID, unsigned int dwSocketIndex, in_addr dwIP, unsigned __int16 wPort);
  void OnLoop();
  void OnLoop_Receipt();
  void ProcessLogFile(unsigned int dwProID, bool bRecv, bool bSend, bool bSystem);
  unsigned int GetCheckRecvTime(unsigned int dwProID, unsigned int dwSocketIndex);

  virtual void UserLoop();
  virtual bool DataAnalysis(
    unsigned int dwProID,
    unsigned int dwClientIndex,
    _MSG_HEADER *pMsgHeader,
    char *pMsg);
  virtual void AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial);
  virtual void CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial);
  virtual bool ExpulsionSocket(unsigned int dwProID, unsigned int dwIndex, unsigned __int8 byReason, void *pvInfo);
  virtual void AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult);

  char m_szSystemName[128];
  unsigned int m_dwUseProcessNum;
  CLogFile m_LogFile;
  CNetProcess m_Process[4];
  CNetProcess *m_pProcess[4];
  bool m_bUseFG;
  char m_szServerName[33];
  char m_szLogPath[128];
};

class __cppobj __declspec(align(8)) CNetworkEX : public CNetWorking
{
public:
  unsigned int dwMonsterPosMiss;
  unsigned int dwPlayerPosMiss;
  unsigned __int8 m_byStatus[4];

  void UserLoop() override;
  bool DataAnalysis(
    unsigned int dwProID,
    unsigned int dwClientIndex,
    _MSG_HEADER *pMsgHeader,
    char *pMsg) override;
  void AcceptClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial) override;
  void CloseClientCheck(unsigned int dwProID, unsigned int dwIndex, unsigned int dwSerial) override;
  void AnsyncConnectComplete(unsigned int dwProID, unsigned int dwIndex, int nResult) override;

  void SetPassablePacket(unsigned int dwProID, unsigned __int8 byHeader1, unsigned __int8 byHeader2);
  void Close(unsigned int dwProID, unsigned int dwSocketIndex, bool bSlowClose, const char *pszLog);

  bool ClientLineAnalysis(unsigned int n, _MSG_HEADER *pMsgHeader, char *pMsg);
  bool AccountLineAnalysis(unsigned int n, _MSG_HEADER *pMsgHeader, char *pMsg);
  bool BillingLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg);
  bool WebAgentLineAnalysis(int n, _MSG_HEADER *pMsgHeader, char *pMsg);

  bool EnterWorldRequest(unsigned int n, _MSG_HEADER *pMsgHeader, char *pBuf);
  bool MoveLobbyRequest(unsigned int n, char *pBuf);
  bool RegedCharRequest(unsigned int n, char *pBuf);
  bool AddCharRequest(unsigned int n, char *pBuf);
  bool DelCharRequest(unsigned int n, char *pBuf);
  bool SelCharRequest(unsigned int n, char *pBuf);
  bool TutorialProcessReport(int n, char *pBuf);
  bool AliveCharRequest(int n, char *pBuf);
  bool NotifyLocalTimeRequest(int n, char *pBuf);
  bool NewPosStartRequest(unsigned int n, char *pBuf);
  bool BaseDownloadRequest(unsigned int n, char *pBuf);
  bool InvenDownloadRequest(unsigned int n, char *pBuf);
  bool CumDownloadRequest(unsigned int n, char *pBuf);
  bool ForceDownloadRequest(unsigned int n, char *pBuf);
  bool QuestDownloadRequest(unsigned int n, char *pBuf);
  bool SpecialDownloadRequest(unsigned int n, char *pBuf);
  bool OtherShapeRequest(unsigned int n, char *pBuf);
  bool Revival(unsigned int n, char *pBuf);
  bool ExitWorldRequest(unsigned int n, char *pBuf);
  bool LinkBoardDownloadRequest(unsigned int n, char *pBuf);
  bool MacroDownLoadRequest(unsigned int n, char *pBuf);
  bool AMP_DownloadRequest(unsigned int n, char *pBuf);
  bool NextPoint(unsigned int n, char *pBuf);
  bool RealMovPosRequest(unsigned int n, char *pBuf);
  bool Stop(unsigned int n, char *pBuf);
  bool GotoBasePortalRequest(unsigned int n, char *pBuf);
  bool GotoAvatorRequest(unsigned int n, char *pBuf);
  bool MovePortalRequest(unsigned int n, char *pBuf);
  bool RegistBindRequest(unsigned int n, char *pBuf);
  bool EnterReturnGateRequest(unsigned int n, char *pBuf);
  bool MoveTypeChangeRequeset(unsigned int n, char *pBuf);
  bool MoveInfoRequeset(unsigned int n, char *pBuf);
  bool PlayerInfoResult(unsigned int n, char *pBuf);
  bool SelectClassRequest(unsigned int n, char *pBuf);
  bool InitClassRequest(unsigned int n, char *pBuf);
  bool InitClassCostRequest(unsigned int n, char *pBuf);
  bool CanSelectClassRequest(unsigned int n, char *pBuf);
  bool SelectPcBangRewardRequest(unsigned int n, char *pBuf);
  bool BuyStoreRequest(unsigned int n, char *pBuf);
  bool SellStoreRequest(unsigned int n, char *pBuf);
  bool StoreListRequest(unsigned int n, char *pBuf);
  bool ExchangeDalantForGoldRequest(unsigned int n, char *pBuf);
  bool ExchangeGoldForDalantRequest(unsigned int n, char *pBuf);
  bool LimitItemNumRequest(unsigned int n, char *pBuf);
  bool TalikRecorverList(unsigned int n, char *pBuf);
  bool PvpCashRecorverWithTalik(unsigned int n, char *pBuf);
  bool PcBangPrimiumCouponRequest(unsigned int n, char *pBuf);
  bool AttackPersonalRequest(unsigned int n, char *pBuf);
  bool AttackSkillRequest(unsigned int n, char *pBuf);
  bool AttackForceRequest(unsigned int n, char *pBuf);
  bool AttackUnitRequest(unsigned int n, char *pBuf);
  bool AttackTestRequest(unsigned int n, char *pBuf);
  bool AttackSiegeRequest(unsigned int n, char *pBuf);
  bool ItemboxTakeRequest(unsigned int n, char *pBuf);
  bool ThrowStorageRequest(unsigned int n, char *pBuf);
  bool UsePotionRequest(unsigned int n, char *pBuf);
  bool EquipPartRequest(unsigned int n, char *pBuf);
  bool EmbellishRequest(unsigned int n, char *pBuf);
  bool OffPartRequest(unsigned int n, char *pBuf);
  bool MakeItemRequest(unsigned int n, char *pBuf);
  bool UpgradeItemRequest(unsigned int n, char *pBuf);
  bool DownGradeItemRequest(unsigned int n, char *pBuf);
  bool AddBagRequest(unsigned int n, char *pBuf);
  bool UseRecoverLossExpItemRequest(unsigned int n, char *pBuf);
  bool CombineItemRequest(unsigned int n, char *pBuf);
  bool ExchangeItemRequest(unsigned int n, char *pBuf);
  bool CombineExItemRequest(unsigned int n, char *pBuf);
  bool CombineExItemAcceptRequest(unsigned int n, char *pBuf);
  bool UseFireCrackerItemRequest(unsigned int n, char *pBuf);
  bool SetItemCheckRequest(unsigned int n, char *pBuf);
  bool UseSoccerBallItemRequest(unsigned int n, char *pBuf);
  bool UseRadarItemRequest(unsigned int n, char *pBuf);
  bool RadarCharListRequest(unsigned int n, char *pBuf);
  bool NPCLinkCheckItemRequest(unsigned int n, char *pBuf);
  bool UseRecallTeleportItemRequest(unsigned int n, char *pBuf);
  bool CharacterRenameCash(unsigned int n, char *pBuf);
  bool TalikCrystalExchangeRequest(unsigned int n, _MSG_HEADER *pHeader, char *pBuf);
  bool ForceInvenChangeRequest(unsigned int n, char *pBuf);
  bool AnimusInvenChangeRequest(unsigned int n, char *pBuf);
  bool AnimusRecallRequest(unsigned int n, char *pBuf);
  bool AnimusReturnRequest(unsigned int n, char *pBuf);
  bool AnimusCommandRequest(unsigned int n, char *pBuf);
  bool AnimusTargetRequest(unsigned int n, char *pBuf);
  bool ResSeparationRequest(unsigned int n, char *pBuf);
  bool ResDivisionRequest(unsigned int n, char *pBuf);
  bool PotionSocketSeparationRequest(unsigned int n, char *pBuf);
  bool PotionSocketDivisionRequest(unsigned int n, char *pBuf);
  bool AlterItemSlotRequest(unsigned int n, char *pBuf);
  bool AlterLinkBoardSlotRequest(unsigned int n, char *pBuf);
  bool PvpRankListRequest(unsigned int n, char *pBuf);
  bool ModeChangeRequest(unsigned int n, char *pBuf);
  bool GustureRequest(unsigned int n, char *pBuf);
  bool AlterWindowInfoRequest(unsigned int n, char *pBuf);
  bool SetTargetObjectRequest(unsigned int n, char *pBuf);
  bool ReleaseTargetObjectRequest(unsigned int n, char *pBuf);
  bool PartyReqBlockReport(unsigned int n, char *pBuf);
  bool WhisperBlockReport(unsigned int n, char *pBuf);
  bool TradeBlockReport(unsigned int n, char *pBuf);
  bool GuildBattleBlockReport(unsigned int n, char *pBuf);
  bool PlayerMacroUpdate(unsigned int n, char *pBuf);
  bool TotalGuildRankRequest(unsigned int n, char *pBuf);
  bool WeeklyGuildRankRequest(unsigned int n, char *pBuf);
  bool SetRaceBossCryMsgRequest(unsigned int n, char *pBuf);
  bool NotifyRaceBossCryMsg(unsigned int n, char *pBuf);
  bool SetGroupTargetObjectRequest(unsigned int n, char *pBuf);
  bool ReleaseGroupTargetObjectRequest(unsigned int n, char *pBuf);
  bool SetGroupMapPointRequest(unsigned int n, char *pBuf);
  bool RequestPatriarchPunishment(unsigned int n, char *pBuf);
  bool RequestTaxRate(unsigned int n, char *pBuf);
  bool RequestChangeTaxRate(unsigned int n, char *pBuf);
  bool RequestUILockInit(unsigned int n, char *pBuf);
  bool RequestUILockUserCertify(unsigned int n, char *pBuf);
  bool RequestUILockUpdateInfo(unsigned int n, char *pBuf);
  bool RequestUILockFindPW(unsigned int n, char *pBuf);
  bool RequestTLLogoutTime(unsigned int n, char *pBuf);
  bool MineStartRequest(unsigned int n, char *pBuf);
  bool MineCancleRequest(unsigned int n, char *pBuf);
  bool ResCuttingRequest(unsigned int n, char *pBuf);
  bool OreIntoBagRequest(unsigned int n, char *pBuf);
  bool CuttingCompleteRequest(unsigned int n, char *pBuf);
  bool PartyJoinInvitation(int n, char *pBuf);
  bool PartyJoinInvitationAnswer(int n, char *pBuf);
  bool PartyJoinApplication(int n, char *pBuf);
  bool PartyJoinApplicatiohAnswer(int n, char *pBuf);
  bool PartyLeaveSelfRequest(int n, char *pBuf);
  bool PartyLeaveCompulsionRequest(int n, char *pBuf);
  bool PartyDisjointRequest(int n, char *pBuf);
  bool PartySuccessionRequest(int n, char *pBuf);
  bool PartyLockRequest(int n, char *pBuf);
  bool AlterPartyLootShareRequest(int n, char *pBuf);
  bool AwayPartyInvitation(int n, char *pBuf);
  bool AwayPartyInvitationAnswer(int n, char *pBuf);
  bool ForceRequest(unsigned int n, char *pBuf);
  bool SkillRequest(unsigned int n, char *pBuf);
  bool ClassSkillRequest(unsigned int n, char *pBuf);
  bool MakeTowerRequest(unsigned int n, char *pBuf);
  bool BackTowerRequest(unsigned int n, char *pBuf);
  bool MakeTrapRequest(unsigned int n, char *pBuf);
  bool DecideRecallRequest(unsigned int n, char *pBuf);
  bool BackTrapRequest(unsigned int n, char *pBuf);
  bool ForceRecallTeleportRequest(unsigned int n, char *pBuf);
  bool SkillRecallTeleportRequest(unsigned int n, char *pBuf);
  bool ClassSkillRecallTeleportRequest(unsigned int n, char *pBuf);
  bool ThrowSkillRequest(unsigned int n, char *pBuf);
  bool ThrowUnitRequest(unsigned int n, char *pBuf);
  bool UnitFrameBuyRequest(unsigned int n, char *pBuf);
  bool UnitSellRequest(unsigned int n, char *pBuf);
  bool UnitPartTuningRequest(unsigned int n, char *pBuf);
  bool UnitFrameRepairRequest(unsigned int n, char *pBuf);
  bool UnitBulletFillRequest(unsigned int n, char *pBuf);
  bool UnitPackFillRequest(unsigned int n, char *pBuf);
  bool UnitDeliveryRequest(unsigned int n, char *pBuf);
  bool UnitReturnRequest(unsigned int n, char *pBuf);
  bool UnitTakeRequest(unsigned int n, char *pBuf);
  bool UnitLeaveRequest(unsigned int n, char *pBuf);
  bool UnitBulletReplaceRequest(unsigned int n, char *pBuf);
  bool SelectWaitedQuestReport(unsigned int n, char *pBuf);
  bool QuestSelectRewardReport(unsigned int n, char *pBuf);
  bool NPCDialogRequest(unsigned int n, char *pBuf);
  bool NPCWatchingRequest(unsigned int n, char *pBuf);
  bool BriefPassReport(unsigned int n, char *pBuf);
  bool NPCQuestRequest(unsigned int n, char *pBuf);
  bool NPCQuestListRequest(unsigned int n, char *pBuf);
  bool QuestGiveupRequest(unsigned int n, char *pBuf);

  bool ChatOperatorRequest(unsigned int n, char *pBuf);
  bool ChatCircleRequest(unsigned int n, char *pBuf);
  bool ChatFarRequest(unsigned int n, char *pBuf);
  bool ChatPartyRequest(unsigned int n, char *pBuf);
  bool ChatRaceRequest(unsigned int n, char *pBuf);
  bool ChatCheatRequest(unsigned int n, char *pBuf);
  bool ChatManageRequest(unsigned int n, char *pBuf);
  bool ChatMgrWhisperRequest(unsigned int n, char *pBuf);
  bool ChatMapRecvYesOrNo(unsigned int n, char *pBuf);
  bool ChatMapRequest(unsigned int n, char *pBuf);
  bool ChatRaceBossRequest(unsigned int n, char *pBuf);
  bool ChatGuildEstSenRequest(unsigned int n, char *pBuf);
  bool ChatRePresentationRequest(unsigned int n, char *pBuf);
  bool ChatAllRecvYesOrNo(unsigned int n, char *pBuf);
  bool ChatAllRequest(unsigned int n, char *pBuf);
  bool ChatGreetingMsg_GM(unsigned int n, char *pBuf);
  bool ChatGreetingMsg_RACE(unsigned int n, char *pBuf);
  bool ChatGreetingMsg_GUILD(unsigned int n, char *pBuf);
  bool ChatTradeRequestMsg(unsigned int n, char *pBuf);
  bool ChatGuildRequest(unsigned int n, char *pBuf);
  bool ChatMultiFarRequest(unsigned int n, char *pBuf);
  bool ChatRaceBossCryRequest(unsigned int n, char *pBuf);
  bool ChatGmNoticeRequest(unsigned int n, char *pBuf);

  bool DTradeAskRequest(unsigned int n, char *pBuf);
  bool DTradeAnswerRequest(unsigned int n, char *pBuf);
  bool DTradeCancleRequest(unsigned int n, char *pBuf);
  bool DTradeLockRequest(unsigned int n, char *pBuf);
  bool DTradeAddRequest(unsigned int n, char *pBuf);
  bool DTradeDelRequest(unsigned int n, char *pBuf);
  bool DTradeBetRequest(unsigned int n, char *pBuf);
  bool DTradeOKRequest(unsigned int n, char *pBuf);
  bool ObjectServerPosRequest(unsigned int n, char *pBuf);

  bool OpenWorldSuccessResult(unsigned int n, char *pMsg);
  bool OpenWorldFailureResult(unsigned int n, char *pMsg);
  bool ForceCloseCommand(unsigned int n, _CLID *pMsg);
  bool TransAccountInform(unsigned int n, char *pMsg);
  bool EnterWorldResult(unsigned int n, _CLID *pMsg);
  bool UILockInitResult(unsigned int n, char *pMsg);
  bool UILockUpdateResult(unsigned int n, char *pMsg);
  bool UILockRefreshResult(unsigned int n, char *pMsg);
  bool CheckIsBlockIPResult(unsigned int n, char *pMsg);
  bool ConEventTotalSalesCheck(int n, char *pBuf);
  bool DisconnectGuildWarCharacterRequest(int n, char *pBuf);
  bool ManageClientLimitRunRequest(char *pBuf);
  bool ManageClientForceExitRequest();
  bool CashDBInfoRecvResult(int n, char *pBuf);
  bool WorldServiceInform(unsigned int n, bool *pMsg);
  bool WorldExitInform(unsigned int n, char *pMsg);
  bool WorldMsgInform(unsigned int n, char *pMsg);
  bool ChatLockCommand(unsigned int n, _CLID *pMsg);

  bool BillingCloseRequest(int n, char *pBuf);
  bool BillingRemaintimePersonal(int n, char *pBuf);
  bool BillingRemaintimePCBang(int n, char *pBuf);
  bool BillingChangeType(int n, char *pBuf);
  bool BillingExpirePersonal(int n, char *pBuf);
  bool BillingExpirePCBang(int n, char *pBuf);
  bool BillingExpireIPOverflow(int n, char *pBuf);
  bool BillingDestroyModule(int n, char *pBuf);
  bool TaiwanBillingUserCertify(int n, char *pBuf);
  bool ChinaBillingChangePrimium(int n, char *pBuf);
  bool ZoneAliveCheckRequest(int n, char *pBuf);

  bool LogInWebAgentServer(unsigned int n, char *pBuf);
  bool SendRaceBossMsgFromWebRequest(int n, char *pBuf);
  bool LogInControllServer(unsigned int n, char *pBuf);
  bool CancelRaceBossSMSMsg(int n, char *pBuf);
  bool ConnectionStatusRequest(int n);
};

extern CNetworkEX g_Network;

void DeCrypt_Move(char *pStr, int nSize, unsigned __int8 byPlus, unsigned __int16 wCryptKey);
