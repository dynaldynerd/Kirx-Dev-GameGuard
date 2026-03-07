#pragma once

#ifndef MAX_PLAYER
#define MAX_PLAYER 2532
#endif

#ifndef MAX_MONSTER
#define MAX_MONSTER 30000
#endif

#ifndef MAX_NPC
#define MAX_NPC 500
#endif

#ifndef MAX_ANIMUS
#define MAX_ANIMUS 500
#endif

#ifndef MAX_TOWER
#define MAX_TOWER 500
#endif

#ifndef MAX_STONE
#define MAX_STONE 3
#endif

#ifndef MAX_KEEPER
#define MAX_KEEPER 1
#endif

#ifndef MAX_TRAP
#define MAX_TRAP 507
#endif

#ifndef MAX_ITEMBOX
#define MAX_ITEMBOX 5064
#endif

#ifndef MAX_PARKING_UNIT
#define MAX_PARKING_UNIT 300
#endif

#ifndef MAX_DARKHOLE
#define MAX_DARKHOLE 50
#endif

#ifndef MAX_GUILD
#define MAX_GUILD 500
#endif

#ifndef CANDIDATE_MGR_INIT_COUNT
#define CANDIDATE_MGR_INIT_COUNT 500
#endif

#ifndef MAX_AUTOMINE
#define MAX_AUTOMINE MAX_PLAYER
#endif

#ifndef MAX_RETURN_GATE
#define MAX_RETURN_GATE 126
#endif

#ifndef MAX_NUCLEAR_BOMB
#define MAX_NUCLEAR_BOMB 9
#endif

// NormalGuildBattle.ini mapcnt=3 and per-map counts: 1PGoalPosCnt=1, 2PGoalPosCnt=1, BallRegenPosCnt=3.
#ifndef MAX_GUILD_BATTLE_FIELD
#define MAX_GUILD_BATTLE_FIELD 3
#endif

#ifndef MAX_GUILD_BATTLE_1P_GOAL_PER_MAP
#define MAX_GUILD_BATTLE_1P_GOAL_PER_MAP 1
#endif

#ifndef MAX_GUILD_BATTLE_2P_GOAL_PER_MAP
#define MAX_GUILD_BATTLE_2P_GOAL_PER_MAP 1
#endif

#ifndef MAX_GUILD_BATTLE_REGEN_PER_MAP
#define MAX_GUILD_BATTLE_REGEN_PER_MAP 3
#endif

#ifndef MAX_GUILD_BATTLE_OBJECTS
#define MAX_GUILD_BATTLE_OBJECTS                                                                      \
  (MAX_GUILD_BATTLE_FIELD *                                                                           \
   (MAX_GUILD_BATTLE_1P_GOAL_PER_MAP + MAX_GUILD_BATTLE_2P_GOAL_PER_MAP + MAX_GUILD_BATTLE_REGEN_PER_MAP + 1))
#endif

#ifndef MAX_TOTAL_CGAMEOBJECT
#define MAX_TOTAL_CGAMEOBJECT                                                                          \
  (MAX_PLAYER + MAX_MONSTER + MAX_NPC + MAX_ANIMUS + MAX_TOWER + MAX_STONE + MAX_KEEPER + MAX_TRAP +    \
   MAX_ITEMBOX + MAX_PARKING_UNIT + MAX_DARKHOLE + MAX_AUTOMINE + MAX_NUCLEAR_BOMB + MAX_RETURN_GATE +  \
   MAX_GUILD_BATTLE_OBJECTS)
#endif

#ifndef MAX_DQS
#define MAX_DQS 12660
#endif

#ifndef MAX_DB_QRY_SYN
#define MAX_DB_QRY_SYN 12660
#endif
