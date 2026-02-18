#pragma once

#include "IdaCompat.h"
#include "CNetIndexList.h"

class  __declspec(align(8)) _DELAY_PROCESS
{
public:
  _DELAY_PROCESS();
  _DELAY_PROCESS(unsigned int dwObjectNum, unsigned int dwTerm);
  virtual ~_DELAY_PROCESS();

  bool Init(unsigned int dwObjectNum, unsigned int dwTerm);
  bool Push(unsigned int dwIndex, unsigned int dwSerial);
  void Delete(unsigned int dwIndex, unsigned int dwSerial);
  void CheckOnLoop();
  virtual void Process(unsigned int dwIndex, unsigned int dwSerial);

  unsigned int m_dwObjectNum;
  unsigned int *m_pdwPushTime;
  unsigned int *m_pdwPushSerial;
  CNetIndexList m_list;
  int m_dwTerm;
};
