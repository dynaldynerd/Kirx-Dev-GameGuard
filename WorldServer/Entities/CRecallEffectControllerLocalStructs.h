#pragma once

#pragma pack(push, 1)
  struct DecideRecallErrorMessage
  {
    char byErr;
    int nCallerMapCode;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct RecallRequestMessage
  {
    unsigned __int16 wRequestID;
    char wszPerformerName[17];
  };
#pragma pack(pop)

  #pragma pack(push, 1)
  struct RecallRequestResultMsg
  {
    char byRetCode;
    char wszDstName[17];
  };
  #pragma pack(pop)

