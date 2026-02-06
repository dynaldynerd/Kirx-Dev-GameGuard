#include "pch.h"

#include "GlobalObjects.h"

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
CMainThread g_Main{};
int gAsci[256]{};
CWnd *g_pFrame = nullptr;

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
