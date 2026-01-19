#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CObject
{
};

class CFile : public CObject
{
};

class __cppobj CMirrorFile : public CFile
{
};
#endif
