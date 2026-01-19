#pragma once

#include "IdaCompat.h"

struct HINSTANCE__;
struct _MSG_HEADER;
struct _socket;
struct _NET_TYPE_PARAM;

class __cppobj CNetwork
{
public:
  virtual ~CNetwork() = default;

  HINSTANCE__ *m_hChinaDLL;
  void (__fastcall *SetDataAnalysisFunc)(bool (__fastcall *)(unsigned int, unsigned int, _MSG_HEADER *, char *));
  void (__fastcall *Release)();
  int (__fastcall *LoadSendMsg)(unsigned int, unsigned int, unsigned __int8 *, unsigned int, char *, unsigned __int16);
  bool (__fastcall *SetNetSystem)(unsigned int, _NET_TYPE_PARAM *, char *, char *);
  void (__fastcall *OnLoop)();
  int (__fastcall *Connect)(unsigned int, unsigned int, unsigned int, unsigned __int16);
  void (__fastcall *CloseSocket)(unsigned int, unsigned int, bool);
  _socket *(__fastcall *GetSocket)(unsigned int, unsigned int);
  bool (__fastcall *RecvS)(char *, int, int *);
  bool (__fastcall *SendS)(char *, int, int *);
};
