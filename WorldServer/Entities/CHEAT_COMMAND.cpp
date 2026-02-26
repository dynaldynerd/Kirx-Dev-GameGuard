#include "pch.h"

#include "CHEAT_COMMAND.h"

#include <cstring>

#include "CheatCommands.h"
#include "CLogFile.h"
#include "CMainThread.h"
#include "CNationSettingManager.h"
#include "CPlayer.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

int s_nWordCount = 0;
char s_wszCheatTokens[6][32]{};
char *s_pwszDstCheat[6] = {
  s_wszCheatTokens[0],
  s_wszCheatTokens[1],
  s_wszCheatTokens[2],
  s_wszCheatTokens[3],
  s_wszCheatTokens[4],
  s_wszCheatTokens[5],
};
char wszRespon[256]{};
CLogFile s_logCheat;

bool CHEAT_COMMAND::operator()(const CHEAT_COMMAND *lhs, const CHEAT_COMMAND *rhs)
{
  return lhs->uiCmdLen > rhs->uiCmdLen;
}

bool AuthorityFilter(CHEAT_COMMAND *pCmd, CPlayer *pOne)
{
  if (pOne)
  {
    if (g_Main.IsReleaseServiceMode())
    {
      const unsigned int useMask = 1u << pOne->m_byUserDgr;
      if ((useMask & static_cast<unsigned int>(pCmd->nUseDegree)) == 0)
      {
        return false;
      }
      if (pOne->m_byUserDgr == 2)
      {
        const unsigned int mgrMask = 1u << pOne->m_bySubDgr;
        if ((mgrMask & static_cast<unsigned int>(pCmd->nMgrDegree)) == 0)
        {
          return false;
        }
      }
    }
  }
  else if ((pCmd->nUseDegree & 0x20) == 0)
  {
    return false;
  }

  return true;
}

bool ProcessCheatCommand(CPlayer *pOne, char *pwszCommand)
{
  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  CHEAT_COMMAND *cheatTable = nationSetting->GetCheatTable();
  CHEAT_COMMAND *pCmd = nullptr;

  if (strchr(pwszCommand, '%'))
  {
    sprintf(wszRespon, "%s >> FAIL(grammar or logic)", pwszCommand);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
    return true;
  }

  for (int j = 0; ; ++j)
  {
    pCmd = &cheatTable[j];
    if (!pCmd->pwszCommand)
    {
      break;
    }
    if (!_strnicmp(pCmd->pwszCommand, pwszCommand, pCmd->uiCmdLen))
    {
      if (AuthorityFilter(pCmd, pOne))
      {
        s_nWordCount = ParsingCommandW(&pwszCommand[pCmd->uiCmdLen], 6, s_pwszDstCheat, 32);
        if (pCmd->fn(pOne))
        {
          WriteCheatLog(pwszCommand, pOne);
          if (pOne)
          {
            sprintf(wszRespon, "%s >> OK", pwszCommand);
            pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
          }
          return true;
        }

        if (pOne)
        {
          sprintf(wszRespon, "%s >> FAIL(grammar or logic)", pwszCommand);
          pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
        }
        return false;
      }

      sprintf(wszRespon, "%s >> ERROR (authority)", pwszCommand);
      pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
      return false;
    }
  }

  if (pOne)
  {
    sprintf(wszRespon, "%s >> ERROR (command)", pwszCommand);
    pOne->SendData_ChatTrans(0, 0xFFFFFFFF, 0xFFu, 0, wszRespon, 0xFFu, nullptr);
  }
  return false;
}

void WriteCheatLog(char *pwszCommand, CPlayer *pOne)
{
  char buffer[1284]{};
  std::memset(buffer, 0, 1280);
  if (pOne)
  {
    const char *charName = pOne->m_Param.GetCharNameA();
    sprintf(buffer, "[ %s ] >> ", charName);
  }
  else
  {
    sprintf(buffer, "[ GM tool ] >> ");
  }

  const int offset = static_cast<int>(strlen_0(buffer));
  W2M(pwszCommand, &buffer[offset], 1280 - offset);
  s_logCheat.Write(buffer);
}

void InitCheatCommand(CHEAT_COMMAND *pCmdList, unsigned __int8 *byCommandSizeList)
{
  for (int j = 0; ; ++j)
  {
    CHEAT_COMMAND *entry = &pCmdList[j];
    if (!entry->pwszCommand)
    {
      break;
    }
    byCommandSizeList[j] = static_cast<unsigned __int8>(strlen_0(entry->pwszCommand));
  }

  const unsigned int korLocalTime = GetKorLocalTime();
  char buffer[144]{};
  sprintf(buffer, "..\\ZoneServerLog\\ServiceLog\\Cheat%u.log", korLocalTime);
  s_logCheat.SetWriteLogFile(buffer, 1, 0, 1, 1);
}
