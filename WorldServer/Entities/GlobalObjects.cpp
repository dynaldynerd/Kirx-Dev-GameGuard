#include "pch.h"

#include "GlobalObjects.h"

#include "CCRFG_SEND_BUFFER.h"
#include "CGameServerDoc.h"

CUserDB g_UserDB[MAX_PLAYER]{};
CPartyPlayer g_PartyPlayer[MAX_PLAYER]{};
CPlayer g_Player[MAX_PLAYER]{};
CPlayer sPlayerDum{};

CMonster *g_Monster = nullptr;
CMerchant *g_NPC = nullptr;
CAnimus *g_Animus = nullptr;
CGuardTower *g_Tower = nullptr;
CHolyStone *g_Stone = nullptr;
CHolyKeeper *g_Keeper = nullptr;
CTrap *g_Trap = nullptr;
CItemBox *g_ItemBox = nullptr;
CParkingUnit *g_ParkingUnit = nullptr;
CDarkHole *g_DarkHole = nullptr;
CGuild *g_Guild = nullptr;
AggroCaculateData g_AggroCaculateData{};
MonsterSetInfoData g_MonsterSetInfoData{};

_R3ENGINE_STATE stState{};
int dword_184A79820 = 0;

CTransportShip g_TransportShip[3]{};
CLogFile g_logSchedule{};
CHolyStoneSystem g_HolySys{};
CWorldSchedule g_WorldSch{};
CGameStatistics g_GameStatistics{};
_ECONOMY_SYSTEM e_EconomySystem{};
_economy_history_data e_EconomyHistory[12]{};
unsigned int e_dwMinCount = 0;
unsigned int e_dwUserCumCount[3]{};
int e_nOreDft = 0;
int e_nTaxDft = 0;
int e_nMgrDft = 0;
CVoteSystem g_VoteSys[3]{};
CMainThread g_Main{};
CNetworkEX g_Network{};
int gAsci[256]{};
CWnd *g_pFrame = nullptr;
CGameServerDoc g_GameServerDoc{};
CGameServerDoc *g_pDoc = &g_GameServerDoc;
CLogFile g_FgLogFile{};
_CCRFG_SEND_BUFFER g_FGSendData{};
_CCRFG_RECV_BUFFER g_FGRecvData{};
_CCRFG_CALLBACK g_pfnCallBack{};
unsigned __int8 g_cbHashVerify[32] = {
  0xB1, 0x77, 0xCC, 0xCA, 0x32, 0x1F, 0x34, 0x80, 0x99, 0x07, 0xF9, 0x42, 0x92, 0x5C, 0x0C, 0x4C,
  0x23, 0x1B, 0x8E, 0xBC, 0xBB, 0x42, 0xCD, 0xFC, 0x9A, 0x24, 0x0B, 0x2C, 0x55, 0x28, 0xA1, 0xF8};
unsigned __int8 g_key[16] = {
  0x49, 0x79, 0xC2, 0x04, 0x32, 0xC2, 0x3D, 0xD4, 0x3D, 0x86, 0xF2, 0x7E, 0x33, 0xDE, 0x43, 0x9D};
unsigned __int8 g_iv[16] = {
  0x08, 0x90, 0xCC, 0x90, 0x2D, 0x69, 0x45, 0x04, 0x10, 0x6C, 0xE4, 0xF9, 0xAA, 0x96, 0xD3, 0x79};

int g_tmpEffectedNum = 0;
_tmp_effected_list g_tmpEffectedList[30]{};
bool (__fastcall *g_TempEffectFunc[150])(CCharacter *, CCharacter *, float, unsigned __int8 *) = {};

float ITEM_ROOT_RATE = 0.0f;
float MINE_SPEED_RATE = 0.0f;
float FORCE_LIVER_ACCUM_RATE = 0.0f;
float MASTERY_GET_RATE = 0.0f;
float ANIMUS_EXP_RATE = 0.0f;
float PLAYER_EXP_RATE = 0.0f;
float PLAYER_LOST_EXP = 0.0f;
float TSVR_ADD_DARKHOLE_REWARD_RATE = 0.0f;
unsigned __int8 Major_Bind_HQ = 0;
unsigned __int8 Major_Sette_Mine_Elan_Map = 0;
unsigned __int8 Major_Scroll_Item = 0;
unsigned __int8 Major_Cash_Item = 0;
unsigned __int8 Major_Add_Character = 0;
unsigned int g_dwCurTime = 0;
