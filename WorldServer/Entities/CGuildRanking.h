#pragma once

#include "IdaCompat.h"

#include <vector>

#include "guild_member_refresh_data.h"

class CLogFile;
struct _worlddb_guild_info;
struct _guild_member_info;
struct _io_money_data;

class  __declspec(align(8)) CGuildRanking
{
public:
  bool Init();
  char Load();
  char UpdateRankAndGrade();
  char LoadAllGuildData(struct _worlddb_guild_info *pkInfo);
  char SetLoadAllGuildInfo(struct _worlddb_guild_info *pkInfo);
  char LoadMemberInfo(
    unsigned int dwMasterSerial,
    unsigned int dwGuildSerial,
    struct _guild_member_info *pkMemberInfo,
    unsigned __int16 *pwMemberCnt);
  char LoadGuildMoneyIOInfo(
    unsigned int dwGuildSerial,
    struct _io_money_data *pkIOInfo,
    int *pnIOMonHisNum);
  char CheckGuildCheckSum(
    unsigned int dwSerial,
    char *wszGuildName,
    long double *dDalant,
    long double *dGold);
  void CheckMaxGuildMoney(
    unsigned int dwGuildSerial,
    char *wszGuildName,
    long double *pdDalant,
    long double *pdGold);
  void SetLogger(CLogFile *pkLogger);
  void ApplyGuildGrade();
  bool ApplyRankInGuild();
  void ClearApplyRankInGuildJobOffset();
  void ClearGuildSerial();
  void ClearGuildGrade();
  unsigned int GetRankInGuildJobOffset();
  void ClearRefreshData();
  unsigned int GetNextGuildSerial();

  unsigned __int8 UpdateGuildRankStep1(char *szData);
  unsigned __int8 UpdateGuildRankStep2(char *szData);
  unsigned __int8 UpdateGuildRankStep3(char *szData);
  unsigned __int8 UpdateGuildRankStep4(char *szData);
  unsigned __int8 UpdateRankinGuildStep1(char *szData);
  unsigned __int8 UpdateRankinGuildStep2(char *szData);
  unsigned __int8 UpdateRankinGuildStep3(char *szData);
  unsigned __int8 UpdateRankinGuildStep4(char *szData);
  unsigned __int8 UpdateRankinGuildStep5(char *szData);
  unsigned __int8 UpdateRankinGuildStep6(char *szData);
  unsigned __int8 UpdateAndSelectGuildGrade(char *szData);

  CLogFile *m_pkLogger;
  unsigned int m_dwCurProcIndex;
  unsigned int m_dwNextProcIndex;
  unsigned int m_dwAllGuildCount;
  std::vector<unsigned long> m_vecAllGuildSerial;
  std::vector<unsigned char> m_vecAllGuildGrade;
  std::vector<_guild_member_refresh_data *> m_vecGuildMemberRefresh;
  int m_nRankInGuildJobOffset;
};
