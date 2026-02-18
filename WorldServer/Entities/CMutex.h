#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CSyncObject
{
};

class  CMutex : public CSyncObject
{
};
#endif
