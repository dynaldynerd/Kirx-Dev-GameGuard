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

extern CUserDB g_UserDB[MAX_PLAYER];
extern CPartyPlayer g_PartyPlayer[MAX_PLAYER];
extern CPlayer g_Player[MAX_PLAYER];

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

extern const char* stState;
extern int dword_184A79820;

extern CTransportShip g_TransportShip[3];
extern CHolyStoneSystem g_HolySys;
extern CWorldSchedule g_WorldSch;
extern CMainThread g_Main;
