#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CObject
{
};

class CFile : public CObject
{
};

class  CMirrorFile : public CFile
{
};
#endif
