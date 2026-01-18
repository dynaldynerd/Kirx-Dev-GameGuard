#include "pch.h"
#include "InternalPacketProcessor.h"

bool InternalPacketProcessor::Handle(uint8_t opcode, uint8_t subcode, const std::vector<uint8_t>& payload)
{
    UNREFERENCED_PARAMETER(opcode);
    UNREFERENCED_PARAMETER(subcode);
    UNREFERENCED_PARAMETER(payload);
    return true;
}
