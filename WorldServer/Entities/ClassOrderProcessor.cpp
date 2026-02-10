#include "pch.h"

#include "ClassOrderProcessor.h"
#include "PatriarchElectProcessor.h"
#include "CandidateMgr.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"

#include <cstring>

ClassOrderProcessor::ClassOrderProcessor()
  : ElectProcessor(_eClassOrderProcessor)
{
  memset(_kSend, 0, sizeof(_kSend));
  _byPtType[0] = 56;
  _byPtType[1] = 17;

  for (int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    for (int classType = 0; classType < 4; ++classType)
    {
      _kSend[raceIndex].body[classType].byClassType = static_cast<unsigned __int8>(classType);
    }
  }
}

ClassOrderProcessor *ClassOrderProcessor::Instance()
{
  PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
  return static_cast<ClassOrderProcessor *>(processor->_kProcessor[ElectProcessor::_eClassOrderProcessor]);
}

void ClassOrderProcessor::SendMsg_QueryAppointResult(
  unsigned __int16 wIndex,
  unsigned __int8 byRet,
  unsigned __int8 byClassType,
  char *pwszAvatorName)
{
  unsigned __int8 pbyType[36]{};
  memcpy_0(pbyType, "8\t", 2u);

  _pt_query_appoint_zocl msg{};
  msg.byRet = byRet;
  msg.byClassType = byClassType;
  strcpy_0(msg.wszAvatorName, pwszAvatorName);

  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, pbyType, reinterpret_cast<char *>(&msg), len);
}

void ClassOrderProcessor::UpdatePacket(unsigned __int8 byRace, unsigned __int8 byClassType)
{
  const unsigned int classIndex = byClassType - 5;
  if (classIndex >= 4)
  {
    return;
  }

  CandidateMgr *mgr = CandidateMgr::Instance();
  CandidateMgr::_candidate_info *group =
    mgr->GetPatriarchGroup(byRace, static_cast<CandidateMgr::_candidate_info::ClassType>(byClassType));
  if (group)
  {
    if (group->eStatus == CandidateMgr::_candidate_info::candidate_discharge)
    {
      _kSend[byRace].body[classIndex].byLevel = 0;
    }
    else
    {
      _kSend[byRace].body[classIndex].byLevel = group->byLevel;
    }

    _kSend[byRace].body[classIndex].byClassType =
      static_cast<unsigned __int8>(static_cast<unsigned __int8>(group->eClassType) - 5);
    _kSend[byRace].body[classIndex].dPvpPoint = group->dPvpPoint;
    strcpy_0(_kSend[byRace].body[classIndex].wszAvatorName, group->wszName);
  }
  else
  {
    _kSend[byRace].body[classIndex].byClassType = static_cast<unsigned __int8>(byClassType - 5);
    _kSend[byRace].body[classIndex].byLevel = static_cast<unsigned __int8>(-1);
  }
}
