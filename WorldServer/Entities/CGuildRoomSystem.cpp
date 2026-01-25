#include "pch.h"

#include "CGuildRoomSystem.h"

CGuildRoomSystem *CGuildRoomSystem::GetInstance()
{
  static CGuildRoomSystem s_instance;
  return &s_instance;
}

bool CGuildRoomSystem::Init()
{
  return true;
}
