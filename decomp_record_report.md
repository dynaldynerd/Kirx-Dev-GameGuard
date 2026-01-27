Decomp record pointer-math report

Goal: Replace _base_fld pointer arithmetic with typed _xxx_fld fields once layouts are confirmed in WorldServerIdaPro/structs.

CMainThread.cpp:931 
- mob message uses record[1].m_dwIndex for byUsingNum; map to the real MobMessage field.
=> _mobmsg_fld* Record
v5 = atoi(Record->m_strCode);
this->m_MobMessage[n].wIndex = v5;
this->m_MobMessage[n].byUsingNum = Record->m_nUsingNum;

CMainThread.cpp:1183-1201
- item table name destinations use record[1]/record[2]/record[3]/m_strCode offsets; replace with per-item-table name field once structs are verified.
=> all corresponding *_fld are commented on "itemNameFiles" variable definition
it just changing the name, so all of them are pointing to this variable: Record.m_strName despite different structure


CMainThread.cpp:1282 => _skill_fld->m_strKorName
- effect name string uses record[3].m_strCode; map to effect-name field.

CMainThread.cpp:1324 => _class_fld->m_strKorName
- class name string uses record[9].m_strCode[48]; map to class-name field.

CMainThread.cpp:1359 => _player_fld->m_strName
- player name string uses record[1] offset; map to player-name field.

CMainThread.cpp:1394 => _monster_fld->m_strName
- monster name string uses record[1] offset; map to monster-name field.

CMainThread.cpp:1429 => _npc_fld->m_strName
- NPC name string uses record[1] offset; map to NPC-name field.

CMainThread.cpp:1464 => _AnimusItem_fld->m_strName
- animus name string uses record[2].m_strCode[60]; map to animus-name field.

CMainThread.cpp:1523 => _UnitPart_fld->m_strName
- unit part name string uses record[2] offset; map to unit-part-name field.

CMainThread.cpp:1559 => _UnitBullet_fld->m_strName
- unit bullet name string uses record[2] offset; map to unit-bullet-name field.

CPotionMgr.cpp:68 => _skill_fld->m_strKorName
- potion effect name uses record[3].m_strCode; map to potion-effect-name field.

CPotionMgr.cpp:103 => _CheckPotion_fld->m_strKOR_name
- potion check name uses record[1] offset; map to potion-check-name field.
