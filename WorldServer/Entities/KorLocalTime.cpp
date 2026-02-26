#include "pch.h"

#include "KorLocalTime.h"

#include <cstdio>
#include <cstdlib>

unsigned long long GetKorLocalTime()
{
    char dateBuf[64];
    char timeBuf[64];
    char buffer[11];

    _strdate_s(dateBuf, sizeof(dateBuf));
    _strtime_s(timeBuf, sizeof(timeBuf));

    buffer[0] = dateBuf[6];
    buffer[1] = dateBuf[7];
    buffer[2] = dateBuf[0];
    buffer[3] = dateBuf[1];
    buffer[4] = dateBuf[3];
    buffer[5] = dateBuf[4];
    buffer[6] = timeBuf[0];
    buffer[7] = timeBuf[1];
    buffer[8] = timeBuf[3];
    buffer[9] = timeBuf[4];
    buffer[10] = '\0';

    return std::strtoull(buffer, nullptr, 10);
}
