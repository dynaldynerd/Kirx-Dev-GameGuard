#pragma once

#include <string>

struct AreaData
{
    std::string strAreaName;
    unsigned int dwRGB;
    unsigned int dwX;
    unsigned int dwY;
    unsigned int dwSound;

    AreaData() : dwRGB(0), dwX(0), dwY(0), dwSound(0) {}
};
