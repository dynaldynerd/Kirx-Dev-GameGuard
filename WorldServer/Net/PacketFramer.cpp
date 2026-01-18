#include "pch.h"
#include "PacketFramer.h"

void PacketFramer::Reset()
{
    m_buffer.clear();
}

void PacketFramer::Append(const void* data, size_t size)
{
    if (data == nullptr || size == 0)
    {
        return;
    }

    const auto* bytes = static_cast<const uint8_t*>(data);
    m_buffer.insert(m_buffer.end(), bytes, bytes + size);
}

bool PacketFramer::TryPop(std::vector<uint8_t>& outPacket)
{
    if (m_buffer.size() < 4)
    {
        return false;
    }

    uint16_t size = static_cast<uint16_t>(m_buffer[0] | (m_buffer[1] << 8));
    if (size < 4)
    {
        return false;
    }

    if (m_buffer.size() < size)
    {
        return false;
    }

    outPacket.assign(m_buffer.begin(), m_buffer.begin() + size);
    m_buffer.erase(m_buffer.begin(), m_buffer.begin() + size);
    return true;
}
