#pragma once

#include <cstdint>
#include <vector>

class PacketFramer
{
public:
    void Reset();
    void Append(const void* data, size_t size);
    bool TryPop(std::vector<uint8_t>& outPacket);

private:
    std::vector<uint8_t> m_buffer;
};
