#include "prmfb.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPrmfb::Nv2aPrmfb(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPrmfb::Read(uint32_t addr, unsigned size)
{
    switch (addr) {
        default:
            Log(LogLevel::Warning, "Nv2aPrmfb: Unimplemented Read %X\n", addr); break;
        }

    return 0;
}

void Nv2aPrmfb::Write(uint32_t addr, uint32_t value, unsigned size)
{
    switch (addr) {
        default:
            Log(LogLevel::Warning, "Nv2aPrmfb: Unimplemented Write %X = %X\n", addr, value); break;
    }
}