#include "prmcio.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

#define VGA_BASE 0x3C0

Nv2aPrmcio::Nv2aPrmcio(Nv2a& nv2a) : m_Nv2a(nv2a)
{
    m_Index = 0;
}


uint32_t Nv2aPrmcio::Read(uint32_t addr, unsigned size)
{
    switch (addr) {
        case NV_PRMCIO_CRX__COLOR: return m_Index;
        case NV_PRMCIO_CR__COLOR: return m_Regs[m_Index];
        default:
            Log(LogLevel::Warning, "Nv2aPrmcio: Unimplemented Read %X\n", addr);
    }

    return 0;
}

void Nv2aPrmcio::Write(uint32_t addr, uint32_t value, unsigned size)
{
    switch (addr) {
        case NV_PRMCIO_CRX__COLOR: m_Index = value; return;
        case NV_PRMCIO_CR__COLOR: m_Regs[m_Index] = value; return;
        default:
            Log(LogLevel::Warning, "Nv2aPrmcio: Unimplemented Write %X = %X\n", addr, value);
    }
}

uint8_t Nv2aPrmcio::GetDisplayBpp()
{
    uint8_t bpp[4] = { 4, 1, 2, 4 };
    return bpp[m_Regs[NV_CIO_CRE_PIXEL_INDEX] & 3];
}

uint16_t Nv2aPrmcio::GetDisplayWidth()
{
    int width = ((int)m_Regs[NV_CIO_CR_OFFSET_INDEX])
        | (0x700 & ((int)m_Regs[NV_CIO_CRE_RPC0_INDEX] << 3))
        | (0x800 & ((int)m_Regs[NV_CIO_CRE_LSR_INDEX] << 6));

    width *= 8;
    width /= GetDisplayBpp();

    return (uint16_t)width;
}

uint16_t Nv2aPrmcio::GetDisplayHeight()
{
    int height = (int)m_Regs[NV_CIO_CR_VDE_INDEX]
        | (((int)m_Regs[NV_CIO_CR_OVL_INDEX] & 0x02) >> 1 << 8)
        | (((int)m_Regs[NV_CIO_CR_OVL_INDEX] & 0x40) >> 6 << 9)
        | (((int)m_Regs[NV_CIO_CRE_LSR_INDEX] & 0x02) >> 1 << 10);

    return (uint16_t)height + 1;
}
