#include "pcrtc.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPcrtc::Nv2aPcrtc(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPcrtc::Read(uint32_t addr, unsigned size)
{
    switch (addr) {
        case NV_PCRTC_START: break;
        default:
            Log(LogLevel::Warning, "Nv2aPcrtc: Unimplemented Read %X\n", addr);
    }

    return m_Regs[addr];
}

void Nv2aPcrtc::Write(uint32_t addr, uint32_t value, unsigned size)
{
    switch (addr) {
        case NV_PCRTC_START: break;
        default:
            Log(LogLevel::Warning, "Nv2aPcrtc: Unimplemented Write %X = %X\n", addr, value);
    }

    m_Regs[addr] = value;
}

uint32_t Nv2aPcrtc::GetFramebufferAddr()
{
    return m_Regs[NV_PCRTC_START];
}
