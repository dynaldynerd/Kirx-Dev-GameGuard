#pragma once

#include <cstdint>
#include <vector>

class InternalPacketProcessor
{
public:
    bool Handle(uint8_t opcode, uint8_t subcode, const std::vector<uint8_t>& payload);
};
