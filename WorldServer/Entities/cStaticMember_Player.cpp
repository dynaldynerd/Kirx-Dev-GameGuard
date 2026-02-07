#include "pch.h"

#include "cStaticMember_Player.h"

#include <cstdlib>

#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "base_fld.h"

cStaticMember_Player *cStaticMember_Player::Instance()
{
  static cStaticMember_Player s_instance;
  return &s_instance;
}

bool cStaticMember_Player::Initialize()
{
  return this->loadLimitExpData();
}

bool cStaticMember_Player::loadLimitExpData()
{
  this->_nMaxLv = static_cast<int>(g_Main.m_tblExp.GetRecordNum());
  if (this->_nMaxLv < 50)
  {
    g_Main.m_logSystemError.Write(
      "cStaticMember_Player::loadLimitExpData() : wrong _nMaxLv[%d]",
      this->_nMaxLv);
    ServerProgramExit("Exp Data Load Error", 0);
  }

  const unsigned __int64 maxLevelCount = static_cast<unsigned __int64>(this->_nMaxLv);
  this->_pLimExp = static_cast<long double *>(operator new[](saturated_mul(maxLevelCount, 8uLL)));
  if (!this->_pLimExp)
  {
    g_Main.m_logSystemError.Write(
      "cStaticMember_Player::loadLimitExpData() : failed allocate _pLimExp[%d]",
      this->_nMaxLv);
    ServerProgramExit("Exp Data Load Error", 0);
  }

  for (int index = 0; index < this->_nMaxLv; ++index)
  {
    _base_fld *record = g_Main.m_tblExp.GetRecord(index);
    if (!record)
    {
      g_Main.m_logSystemError.Write("CPlayer::SetStaticMember() : %d Exp Data..NULL", index);
      ServerProgramExit("Exp Data Load Error", 0);
    }
    const double expValue = atof(reinterpret_cast<const char *>(&record[1]));
    this->_pLimExp[index] = expValue;
  }

  return true;
}
