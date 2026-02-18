#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CObject
{
};

class CFile : public CObject
{
};

class COleStreamFile : public CFile
{
};

class  CMonikerFile : public COleStreamFile
{
};
#endif
