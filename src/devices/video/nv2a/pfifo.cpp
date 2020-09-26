#include "pfifo.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPfifo::Nv2aPfifo(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint8_t Nv2aPfifo::GetCurrentChannelId()
{
    return GET_MASK(m_Regs[NV_PFIFO_CACHE1_PUSH1], NV_PFIFO_CACHE1_PUSH1_CHID);
}

uint8_t Nv2aPfifo::GetChannelMode(uint8_t channel_id)
{
    return m_Regs[NV_PFIFO_MODE] & (1 << channel_id) ? 1 : 0;
}

uint32_t Nv2aPfifo::Read(uint32_t addr, unsigned size)
{
    if (addr == NV_PFIFO_RUNOUT_STATUS) {
        return NV_PFIFO_RUNOUT_STATUS_LOW_MARK;
    }

    return m_Regs[addr];

}

void Nv2aPfifo::Write(uint32_t addr, uint32_t value, unsigned size)
{
    switch (addr) {
        case NV_PFIFO_INTR_0: m_Regs[NV_PFIFO_INTR_0] &= ~value; m_Nv2a.UpdateIrq(); return;
        case NV_PFIFO_INTR_EN_0: m_Nv2a.UpdateIrq(); break;
    }

    m_Regs[addr] = value;
}