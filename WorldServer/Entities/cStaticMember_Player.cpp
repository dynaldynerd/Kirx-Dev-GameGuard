#include "pch.h"

#include "cStaticMember_Player.h"

cStaticMember_Player *cStaticMember_Player::Instance()
{
  static cStaticMember_Player s_instance;
  return &s_instance;
}

bool cStaticMember_Player::Initialize()
{
  return true;
}
