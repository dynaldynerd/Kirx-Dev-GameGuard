#pragma once

#include "IdaCompat.h"

// Read-only entity list entries used by EBP files.
#pragma pack(push, 2)
struct __unaligned __declspec(align(2)) _READ_MAP_ENTITIES_LIST
{
  unsigned __int16 ID;
  float Scale;
  float Pos[3];
  float RotX;
  float RotY;
  __int16 BBmin[3];
  __int16 BBmax[3];
};
#pragma pack(pop)

struct _READ_SOUND_ENTITY_LIST
{
  char name[64];
};

struct _READ_SOUND_ENTITIES_LIST
{
  unsigned __int16 id;
  unsigned __int16 event_time;
  unsigned int flag;
  float scale;
  float attn;
  float pos[3];
  float box_scale[3];
  float box_attn;
  float box_rot_x;
  float box_rot_y;
  unsigned int spare;
};
