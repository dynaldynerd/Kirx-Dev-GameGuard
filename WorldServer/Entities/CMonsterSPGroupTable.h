#pragma once

#include "IdaCompat.h"

class CRecordData;
struct _monster_sp_group;

/* 1304 */
class  CMonsterSPGroupTable
{
public:
  CMonsterSPGroupTable();
  ~CMonsterSPGroupTable();

  bool Create(
    CRecordData *monsterTable,
    CRecordData *monsterBaseSP,
    CRecordData *effectData0,
    CRecordData *effectData1,
    CRecordData *effectData2);
  _monster_sp_group *GetRecord(unsigned int dwIndex);
  _monster_sp_group *GetRecord(char *szCode);

  unsigned int m_dwRecordNum;
  _monster_sp_group *m_pRecordData;
};

