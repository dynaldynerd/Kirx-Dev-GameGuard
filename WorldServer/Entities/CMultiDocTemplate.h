#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CObject
{
};

class CCmdTarget : public CObject
{
};

class CDocTemplate : public CCmdTarget
{
};

class  CMultiDocTemplate : public CDocTemplate
{
};
#endif
