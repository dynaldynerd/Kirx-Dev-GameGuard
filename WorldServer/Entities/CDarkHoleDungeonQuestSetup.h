#pragma once

#include "IdaCompat.h"

class strFILE;
struct _dh_quest_setup;
struct _dh_mission_setup;
struct _dh_job_setup;

/* 6045 */
struct  __error_info
{
  __error_info();
  void init();
  void SetFileName(const char *pszFileName);
  void SetQuestTitle(const char *pszTitle);

  char szFileName[128];
  char szQuestTitle[128];
  int nLine;
  char szNextStr[1280];
  char szLastSection[128];
  char szLastCommand[128];
  char szLoadErrMsg[256];
};

/* 6043 */
class  __declspec(align(8)) CDarkHoleDungeonQuestSetup
{
public:
  CDarkHoleDungeonQuestSetup();
  virtual ~CDarkHoleDungeonQuestSetup();

  bool SetupQuest(char *pszQuestFileName);
  char *GetErrorMsg();
  _dh_quest_setup *GetQuestSetupPtr(unsigned int dwQuestIndex);

  bool _Analysis_Quest_Setting(strFILE *fstr);
  bool _Analysis_Quest_Condition(strFILE *fstr);
  bool _Analysis_Mission_Setting(strFILE *fstr);
  bool _Analysis_Mission_Condition(strFILE *fstr);
  bool _Analysis_Job_Setting(strFILE *fstr);
  bool _Analysis_Job_Condition(strFILE *fstr);
  bool _LastCheckScipt(char *pszQuestFileName);

  int m_nLoadQuest;
  _dh_quest_setup *m_QuestSetup[100];
  char m_szLoadErrMsg[256];
  char m_szLoadFileName[256];
  int m_nCurCondiKind;
  _dh_quest_setup *m_pCurLoadQuest;
  _dh_mission_setup *m_pCurLoadMission;
  _dh_job_setup *m_pCurLoadJob;
  __error_info m_ErrorInfo;
};

