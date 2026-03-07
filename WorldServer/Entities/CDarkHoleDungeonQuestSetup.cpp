#include "pch.h"

#include "CDarkHoleDungeonQuestSetup.h"
#include "DarkHoleQuestCommands.h"
#include "CMapData.h"
#include "strFILE.h"

#include <cstdio>

namespace
{
  char szMsg[512]{};
  static const char kDHQuestSectionStart[] = "###";
  static const char kDHMissionSectionStart[] = "##";
  static const char kDHJobSectionStart[] = "#";
  static const char kDHConditionStart[] = "*";
  static const char *szEOF = "[END_OF_FILE]";
  static unsigned int nEOFLen;
  static unsigned int _S1_1 = 0;
}

__error_info::__error_info()
{

  init();
}

void __error_info::init()
{

  memset_0(this, 0, sizeof(__error_info));
}

void __error_info::SetFileName(const char *pszFileName)
{

  strcpy_0(szFileName, pszFileName);
}

void __error_info::SetQuestTitle(const char *pszTitle)
{

  strcpy_0(szQuestTitle, pszTitle);
}

CDarkHoleDungeonQuestSetup::CDarkHoleDungeonQuestSetup()
  : m_ErrorInfo()
{

  m_nLoadQuest = 0;
  m_nCurCondiKind = -1;
  m_pCurLoadQuest = nullptr;
  m_pCurLoadMission = nullptr;
  m_pCurLoadJob = nullptr;
  m_szLoadErrMsg[0] = '\0';
}

CDarkHoleDungeonQuestSetup::~CDarkHoleDungeonQuestSetup() = default;

char *CDarkHoleDungeonQuestSetup::GetErrorMsg()
{

  sprintf(
    szMsg,
    "FILE  : %s \r\nTITLE : %s \r\nLINE  : %d \r\n",
    m_ErrorInfo.szFileName,
    m_ErrorInfo.szQuestTitle,
    m_ErrorInfo.nLine);
  return szMsg;
}

bool CDarkHoleDungeonQuestSetup::SetupQuest(char *pszQuestFileName)
{

  if ((_S1_1 & 1) == 0)
  {
    _S1_1 |= 1u;
    nEOFLen = static_cast<unsigned int>(strlen_0(szEOF));
  }

  bool foundEOF = false;
  strcpy_0(m_szLoadFileName, pszQuestFileName);
  m_szLoadErrMsg[0] = '\0';
  m_ErrorInfo.init();
  m_ErrorInfo.SetFileName(pszQuestFileName);
  strFILE fstr;
  fstr.load(pszQuestFileName);
  if (m_nLoadQuest < 100)
  {
    char poutszWord[1304]{};
    while (fstr.word(poutszWord))
    {
      if (!strcmp_0(poutszWord, kDHQuestSectionStart))
      {
        if (!_Analysis_Quest_Setting(&fstr))
          break;
        m_nCurCondiKind = 0;
      }
      else if (!strcmp_0(poutszWord, kDHMissionSectionStart))
      {
        if (!_Analysis_Mission_Setting(&fstr))
          break;
        m_nCurCondiKind = 1;
      }
      else if (!strcmp_0(poutszWord, kDHJobSectionStart))
      {
        if (!_Analysis_Job_Setting(&fstr))
          break;
        m_nCurCondiKind = 2;
      }
      else if (!strcmp_0(poutszWord, kDHConditionStart))
      {
        if (m_nCurCondiKind)
        {
          if (m_nCurCondiKind == 1)
          {
            if (!_Analysis_Mission_Condition(&fstr))
              break;
          }
          else if (m_nCurCondiKind != 2 || !_Analysis_Job_Condition(&fstr))
          {
            break;
          }
        }
        else if (!_Analysis_Quest_Condition(&fstr))
        {
          break;
        }
      }
      else if (!strncmp_0(poutszWord, szEOF, nEOFLen))
      {
        foundEOF = true;
        break;
      }
    }
  }
  else
  {
    sprintf(m_szLoadErrMsg, "excess of the number of quests >> max : %d", 100);
  }

  if (foundEOF)
  {
    if (!_LastCheckScipt(pszQuestFileName))
      return _false(&fstr, this);
    ++m_nLoadQuest;
  }
  return foundEOF;
}

_dh_quest_setup *CDarkHoleDungeonQuestSetup::GetQuestSetupPtr(unsigned int dwQuestIndex)
{
  if (dwQuestIndex < static_cast<unsigned int>(m_nLoadQuest))
    return m_QuestSetup[dwQuestIndex];
  return nullptr;
}

bool CDarkHoleDungeonQuestSetup::_Analysis_Quest_Setting(strFILE *fstr)
{

  _dh_quest_setup *quest = (_dh_quest_setup *)operator new(4608);
  if (quest)
  {
    new (quest) _dh_quest_setup();
  }
  else
  {
    quest = nullptr;
  }
  m_QuestSetup[m_nLoadQuest] = quest;
  m_pCurLoadQuest = m_QuestSetup[m_nLoadQuest];
  char poutszWord[144]{};
  if (fstr->word(poutszWord))
  {
    if (strlen_0(poutszWord) <= 32)
    {
      strcpy_0(m_pCurLoadQuest->szQuestTitle, poutszWord);
      m_ErrorInfo.SetQuestTitle(poutszWord);
      return true;
    }
    sprintf(m_szLoadErrMsg, "quest title (%s) size over .. must be set up less than %d", poutszWord, 32);
    return _false(fstr, this);
  }
  sprintf(m_szLoadErrMsg, "quest title load error");
  return _false(fstr, this);
}

bool CDarkHoleDungeonQuestSetup::_Analysis_Quest_Condition(strFILE *fstr)
{

  char poutszWord[136]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, this);
  auto fn = SearchQuestCommandFn(poutszWord);
  if (fn)
  {
    char source[132]{};
    const bool ok = fn(fstr, this, source);
    if (!ok)
      strcpy_0(m_szLoadErrMsg, source);
    return ok;
  }
  sprintf(m_szLoadErrMsg, "[QUEST] no definded key >> %s", poutszWord);
  return false;
}

bool CDarkHoleDungeonQuestSetup::_Analysis_Mission_Setting(strFILE *fstr)
{

  if (!m_pCurLoadQuest)
    return _false(fstr, this);
  _dh_mission_setup *mission = (_dh_mission_setup *)operator new(4488);
  if (mission)
  {
    new (mission) _dh_mission_setup();
  }
  else
  {
    mission = nullptr;
  }
  m_pCurLoadQuest->EmbMissionSetup[m_pCurLoadQuest->nEmbMissionSetupNum] = mission;
  m_pCurLoadMission = m_pCurLoadQuest->EmbMissionSetup[m_pCurLoadQuest->nEmbMissionSetupNum++];
  char poutszWord[144]{};
  if (fstr->word(poutszWord))
  {
    if (strlen_0(poutszWord) <= 32)
    {
      strcpy_0(m_pCurLoadMission->szMissionTitle, poutszWord);
      return true;
    }
    sprintf(m_szLoadErrMsg, "mission title (%s) size over .. must be set up less than %d", poutszWord, 32);
    return _false(fstr, this);
  }
  sprintf(m_szLoadErrMsg, "mission title load error");
  return _false(fstr, this);
}

bool CDarkHoleDungeonQuestSetup::_Analysis_Mission_Condition(strFILE *fstr)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, this);
  auto fn = SearchMissionCommandFn(poutszWord);
  if (fn)
  {
    char source[132]{};
    const bool ok = fn(fstr, this, source);
    if (!ok)
      strcpy_0(m_szLoadErrMsg, source);
    return ok;
  }
  sprintf(m_szLoadErrMsg, "[MISSION] no definded key >> %s", poutszWord);
  return false;
}

bool CDarkHoleDungeonQuestSetup::_Analysis_Job_Setting(strFILE *fstr)
{

  if (!m_pCurLoadQuest || !m_pCurLoadMission)
    return _false(fstr, this);
  _dh_job_setup *job = (_dh_job_setup *)operator new(176);
  if (job)
  {
    new (job) _dh_job_setup();
  }
  else
  {
    job = nullptr;
  }
  m_pCurLoadMission->EmbJobSetup[m_pCurLoadMission->nEmbJobSetupNum] = job;
  m_pCurLoadJob = m_pCurLoadMission->EmbJobSetup[m_pCurLoadMission->nEmbJobSetupNum++];
  char poutszWord[144]{};
  if (fstr->word(poutszWord))
  {
    if (strlen_0(poutszWord) <= 32)
    {
      strcpy_0(m_pCurLoadJob->szJobTitle, poutszWord);
      return true;
    }
    sprintf(m_szLoadErrMsg, "job title (%s) size over .. must be set up less than %d", poutszWord, 32);
    return _false(fstr, this);
  }
  sprintf(m_szLoadErrMsg, "job title load error");
  return _false(fstr, this);
}

bool CDarkHoleDungeonQuestSetup::_Analysis_Job_Condition(strFILE *fstr)
{

  char poutszWord[136]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, this);
  auto fn = SearchJobCommandFn(poutszWord);
  if (fn)
  {
    char source[132]{};
    const bool ok = fn(fstr, this, source);
    if (!ok)
      strcpy_0(m_szLoadErrMsg, source);
    return ok;
  }
  sprintf(m_szLoadErrMsg, "[JOB] no definded key >> %s", poutszWord);
  return false;
}

bool CDarkHoleDungeonQuestSetup::_LastCheckScipt(char *pszQuestFileName)
{

  _dh_quest_setup *quest = m_QuestSetup[m_nLoadQuest];
  for (int j = 0; j < quest->nEmbMissionSetupNum; ++j)
  {
    _dh_mission_setup *mission = quest->EmbMissionSetup[j];
    if (!strcmp_0(mission->szMissionTitle, quest->szStartMissionBuffer))
    {
      quest->pStartMissionSetup = mission;
      break;
    }
  }
  if (!quest->pStartMissionSetup)
  {
    sprintf(m_szLoadErrMsg, "can not search a start mission >> %s", pszQuestFileName);
    return _false(nullptr, this);
  }

  for (int j = 0; j < quest->nEmbMissionSetupNum; ++j)
  {
    _dh_mission_setup *mission = quest->EmbMissionSetup[j];
    if (mission->byResultType == 2)
    {
      if (!mission->pszNextMissionTitle)
      {
        sprintf(m_szLoadErrMsg, "next mission title must be defined if result type is NEXT  >> %s", mission->szMissionTitle);
        return _false(nullptr, this);
      }
      mission->pNextMission = quest->SearchMissionFromTitle(mission->pszNextMissionTitle);
      if (!mission->pNextMission)
      {
        sprintf(m_szLoadErrMsg, "can not search a next mission >> %s", mission->pszNextMissionTitle);
        return _false(nullptr, this);
      }
    }
  }

  for (int j = 0; j < quest->nEmbMissionSetupNum; ++j)
  {
    _dh_mission_setup *mission = quest->EmbMissionSetup[j];
    if (!mission->dwLimTimeMSec)
    {
      sprintf(m_szLoadErrMsg, "time-limit must be defined by mission script >> mission: %s", mission->szMissionTitle);
      return _false(nullptr, this);
    }
  }

  for (int j = 0; j < quest->nEmbMissionSetupNum; ++j)
  {
    _dh_mission_setup *mission = quest->EmbMissionSetup[j];
    if (!mission->pStartDummy)
    {
      sprintf(m_szLoadErrMsg, "start dummy is not defined");
      return _false(nullptr, this);
    }
    if (mission->pAreaDummy)
    {
      float *centerPos = mission->pStartDummy->m_fCenterPos;
      unsigned int lineIndex = mission->pAreaDummy->m_wLineIndex;
      CExtDummy *dummyTable = &quest->pUseMap->m_Dummy;
      if (!dummyTable->IsInBBox(lineIndex, centerPos))
      {
        sprintf(
          m_szLoadErrMsg,
          "%s start dummy is outside the limits of %s quest dummy >> %s",
          mission->pStartDummy->m_szCode,
          mission->pAreaDummy->m_szCode,
          reinterpret_cast<const char *>(quest));
        return _false(nullptr, this);
      }
    }
  }
  return true;
}
