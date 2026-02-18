#pragma once

#include "IdaCompat.h"

struct _message
{
  _message();
  ~_message();
  void SetMsg(unsigned int message, unsigned int key1, unsigned int key2, unsigned int key3);
  unsigned int GetMessageA() const;
  unsigned int GetKey1() const;

  unsigned int dwMessage;
  unsigned int dwKey1;
  unsigned int dwKey2;
  unsigned int dwKey3;
  _message *pNext;
  _message *pPrev;
};
