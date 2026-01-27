#include "pch.h"
#include "COreAmountMgr.h"

COreAmountMgr *COreAmountMgr::Instance()
{
    static COreAmountMgr instance;
    return &instance;
}

bool COreAmountMgr::LoadINI()
{
    return true;
}

void COreAmountMgr::InitRemainOreAmount(unsigned int dwRemain, unsigned int dwInit)
{
}

void COreAmountMgr::InitTransferOre(int nTransfer, int nInit)
{
}
