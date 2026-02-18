#pragma once

#include "IdaCompat.h"

struct _SCALE_TRACK;
struct _POS_TRACK;
struct _ROT_TRACK;

#pragma pack(push, 1)
struct _E_ENTRY
{
  unsigned int offset;
  unsigned int size;
};

struct _ENTITY_FILE_HEADER
{
  _E_ENTRY CompHeader;
  _E_ENTRY Vertex;
  _E_ENTRY VColor;
  _E_ENTRY UV;
  _E_ENTRY Face;
  _E_ENTRY FaceId;
  _E_ENTRY VertexId;
  _E_ENTRY MatGroup;
  _E_ENTRY Object;
  _E_ENTRY Track;
};

struct _ANI_OBJECT
{
  char ObjectName[64];
  char ParentName[64];
  unsigned __int16 flag;
  unsigned __int16 parent;
  int frames;
  int Pos_cnt;
  int Rot_cnt;
  int Scale_cnt;
  float scale[3];
  float scale_quat[4];
  float pos[3];
  float quat[4];
  _SCALE_TRACK *Scale;
  _POS_TRACK *Pos;
  _ROT_TRACK *Rot;
  unsigned __int8 AniFrameCache;
  float s_matrix[4][4];
  float now_frame;
  float f_matrix[4][4];
};
#pragma pack(pop)
