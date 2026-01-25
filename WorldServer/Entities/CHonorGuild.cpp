#include "pch.h"

#include "CHonorGuild.h"

CHonorGuild *CHonorGuild::Instance()
{
  static CHonorGuild s_instance;
  return &s_instance;
}

bool CHonorGuild::Init()
{
  return true;
}
