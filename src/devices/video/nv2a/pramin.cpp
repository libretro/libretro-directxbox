#include "pramin.h"
#include "nv2a.h"
#include <cassert>

Nv2aPramin::Nv2aPramin(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPramin::Read(uint32_t addr, unsigned size)
{
    return *((uint32_t*)&m_Data[addr]);
}

void Nv2aPramin::Write(uint32_t addr, uint32_t value, unsigned size)
{
    *((uint32_t*)&m_Data[addr]) = value;
}