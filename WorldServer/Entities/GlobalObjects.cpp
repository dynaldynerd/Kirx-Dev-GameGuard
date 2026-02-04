#include "pch.h"

#include "GlobalObjects.h"

CUserDB g_UserDB[MAX_PLAYER]{};
CPartyPlayer g_PartyPlayer[MAX_PLAYER]{};
CPlayer g_Player[MAX_PLAYER]{};

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
