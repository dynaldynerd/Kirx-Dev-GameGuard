#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CSyncObject
{
};

class __cppobj CMutex : public CSyncObject
{
};
#endif
