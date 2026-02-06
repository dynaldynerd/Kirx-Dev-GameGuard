#include "pch.h"

#include "ClassOrderProcessor.h"

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
