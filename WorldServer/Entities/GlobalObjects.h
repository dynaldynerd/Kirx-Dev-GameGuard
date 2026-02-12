#pragma once

#include "CMainThread.h"
#include "GlobalObjectDefs.h"
#include "CUserDB.h"
#include "CPartyPlayer.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CMerchant.h"
#include "CAnimus.h"
#include "CGuardTower.h"
#include "CHolyStone.h"
#include "CHolyKeeper.h"
#include "CTrap.h"
#include "CItemBox.h"
#include "CParkingUnit.h"
#include "CDarkHole.h"
#include "CGuild.h"
#include "AggroCaculateData.h"
#include "MonsterSetInfoData.h"
#include "CTransportShip.h"
#include "CHolyStoneSystem.h"
#include "CWorldSchedule.h"
#include "CGameStatistics.h"
#include "ECONOMY_SYSTEM.h"
#include "economy_history_data.h"
#include "R3EngineState.h"
#include "CNetworkEX.h"
#include "CVoteSystem.h"

class CWnd;
class CGameServerDoc;
struct _CCRFG_SEND_BUFFER;
struct _CCRFG_RECV_BUFFER;
struct _CCRFG_CALLBACK;

extern CUserDB g_UserDB[MAX_PLAYER];
extern CPartyPlayer g_PartyPlayer[MAX_PLAYER];
extern CPlayer g_Player[MAX_PLAYER];
extern CPlayer sPlayerDum;

extern CMonster *g_Monster;
extern CMerchant *g_NPC;
extern CAnimus *g_Animus;
extern CGuardTower *g_Tower;
extern CHolyStone *g_Stone;
extern CHolyKeeper *g_Keeper;
extern CTrap *g_Trap;
extern CItemBox *g_ItemBox;
extern CParkingUnit *g_ParkingUnit;
extern CDarkHole *g_DarkHole;
extern CGuild *g_Guild;
extern AggroCaculateData g_AggroCaculateData;
extern class MonsterSetInfoData g_MonsterSetInfoData;

extern _R3ENGINE_STATE stState;
extern int dword_184A79820;
extern int dword_184A79824;
extern int gAsci[256];

extern CTransportShip g_TransportShip[3];
extern CLogFile g_logSchedule;
extern CHolyStoneSystem g_HolySys;
extern CWorldSchedule g_WorldSch;
extern CGameStatistics g_GameStatistics;
extern _ECONOMY_SYSTEM e_EconomySystem;
extern _economy_history_data e_EconomyHistory[12];
extern unsigned int e_dwMinCount;
extern unsigned int e_dwUserCumCount[3];
extern int e_nOreDft;
extern int e_nTaxDft;
extern int e_nMgrDft;
extern CVoteSystem g_VoteSys[3];
extern CMainThread g_Main;
extern CNetworkEX g_Network;
extern CWnd *g_pFrame;
extern CGameServerDoc *g_pDoc;
extern CLogFile g_FgLogFile;
extern _CCRFG_SEND_BUFFER g_FGSendData;
extern _CCRFG_RECV_BUFFER g_FGRecvData;
extern _CCRFG_CALLBACK g_pfnCallBack;
extern unsigned __int8 g_cbHashVerify[32];
extern unsigned __int8 g_key[16];
extern unsigned __int8 g_iv[16];

extern int g_tmpEffectedNum;
extern _tmp_effected_list g_tmpEffectedList[30];
extern bool (__fastcall *g_TempEffectFunc[150])(CCharacter *pActChar, CCharacter *pTargetChar, float fEffectValue, unsigned __int8 *byRet);

extern float ITEM_ROOT_RATE;
extern float MINE_SPEED_RATE;
extern float FORCE_LIVER_ACCUM_RATE;
extern float MASTERY_GET_RATE;
extern float ANIMUS_EXP_RATE;
extern float PLAYER_EXP_RATE;
extern float PLAYER_LOST_EXP;
extern float TSVR_ADD_DARKHOLE_REWARD_RATE;
extern unsigned __int8 Major_Bind_HQ;
extern unsigned __int8 Major_Sette_Mine_Elan_Map;
extern unsigned __int8 Major_Scroll_Item;
extern unsigned __int8 Major_Cash_Item;
extern unsigned __int8 Major_Add_Character;
extern unsigned int g_dwCurTime;
