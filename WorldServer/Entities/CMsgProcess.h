#pragma once

#include "IdaCompat.h"
#include "CMsgData.h"

/* 1269 */
class  CMsgProcess : public CMsgData
{
public:
  CMsgProcess();
  explicit CMsgProcess(int nObjNum);
  virtual ~CMsgProcess() = default;
  void ProcessMessage(_message *pMsg) override;
};

