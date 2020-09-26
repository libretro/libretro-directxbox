#include "prma.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPrma::Nv2aPrma(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPrma::Read(uint32_t addr, unsigned size)
{
    switch (addr) {
        default:
            Log(LogLevel::Warning, "Nv2aPrma: Unimplemented Read %X\n", addr); break;
        }

    return 0;
}

void Nv2aPrma::Write(uint32_t addr, uint32_t value, unsigned size)
{
    switch (addr) {
        default:
            Log(LogLevel::Warning, "Nv2aPrma: Unimplemented Write %X = %X\n", addr, value); break;
    }
}