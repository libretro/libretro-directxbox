#include "pmc.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPmc::Nv2aPmc(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPmc::Read(uint32_t addr, unsigned size)
{
    switch (addr) {
        default:
            Log(LogLevel::Warning, "Nv2aPmc: Unimplemented Read %X\n", addr); break;
        }

    return 0;
}

void Nv2aPmc::Write(uint32_t addr, uint32_t value, unsigned size)
{
    switch (addr) {
        default:
            Log(LogLevel::Warning, "Nv2aPmc: Unimplemented Write %X = %X\n", addr, value); break;
    }
}