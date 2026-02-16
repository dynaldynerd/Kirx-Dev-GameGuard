#pragma once

  struct AttackAccessor
  {
    _attack_param *m_pp;
    CCharacter *m_pAttChar;
    bool m_bIsCrtAtt;
    bool m_bActiveSucc;
    int m_nDamagedObjNum;
    _be_damaged_char m_DamList[30];
    bool m_bFailure;
  };

  struct __unaligned __declspec(align(1)) _animus_realmove_packet
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wObjIndex;
    unsigned int dwObjSerial;
    short posAndTarget[5];
    char byLevel;
    unsigned __int16 wContEffect;
    unsigned int dwMasterSerial;
    float fMoveSpeed;
  };

  struct __unaligned __declspec(align(1)) _animus_fixpos_packet
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wObjIndex;
    unsigned int dwObjSerial;
    short pos[3];
    char byLevel;
    unsigned __int16 wContEffect;
    unsigned int dwMasterSerial;
  };

