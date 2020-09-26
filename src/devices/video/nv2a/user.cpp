#include "user.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aUser::Nv2aUser(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aUser::Read(uint32_t addr, unsigned size)
{
    uint8_t channel_id = (addr >> 16) & 0xFF;

    // Do nothing if not accessing the currently active channel
    if (channel_id != m_Nv2a.Pfifo.GetCurrentChannelId()) {
        return 0;
    }

    // Do nothing if using PIO instead of DMA
    if (!m_Nv2a.Pfifo.GetChannelMode(channel_id)) {
        Log(LogLevel::Warning, "Nv2aUser: PIO Mode unsupported\n");
        return 0;
    }

    switch (addr & 0xFFFF) {
        case NV_USER_DMA_PUT: return m_Nv2a.Pfifo.Read(NV_PFIFO_CACHE1_DMA_PUT, size);
        case NV_USER_DMA_GET: return m_Nv2a.Pfifo.Read(NV_PFIFO_CACHE1_DMA_GET, size);
        case NV_USER_REF:     return m_Nv2a.Pfifo.Read(NV_PFIFO_CACHE1_REF, size);
        default: Log(LogLevel::Warning, "Nv2aUser: Unimplemented Read %X\n", addr);
    }

    return m_Regs[addr];
}

void Nv2aUser::Write(uint32_t addr, uint32_t value, unsigned size)
{
    uint8_t channel_id = (addr >> 16) & 0xFF;

    // Do nothing if not accessing the currently active channel
    if (channel_id != m_Nv2a.Pfifo.GetCurrentChannelId()) {
        return;
    }

    // Do nothing if using PIO instead of DMA
    if (!m_Nv2a.Pfifo.GetChannelMode(channel_id)) {
        Log(LogLevel::Warning, "Nv2aUser: PIO Mode unsupported\n");
        return;
    }

    switch (addr & 0xFFFF) {
        case NV_USER_DMA_PUT: m_Nv2a.Pfifo.Write(NV_PFIFO_CACHE1_DMA_PUT, value, size);
        case NV_USER_DMA_GET: m_Nv2a.Pfifo.Write(NV_PFIFO_CACHE1_DMA_GET, value, size);
        case NV_USER_REF:     m_Nv2a.Pfifo.Write(NV_PFIFO_CACHE1_REF, value, size);
        default: Log(LogLevel::Warning, "Nv2aUser: Unimplemented Write %X = %X\n", addr, value);
    }

    m_Regs[addr] = value;
}