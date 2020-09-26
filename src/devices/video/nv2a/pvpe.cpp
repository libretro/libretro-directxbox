#include "pvpe.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPvpe::Nv2aPvpe(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPvpe::Read(uint32_t addr, unsigned size)
{
   switch (addr)
   {
      default:
         Log(LogLevel::Warning, "Nv2aPvpe: Unimplemented Read %X\n", addr); break;
   }

   return 0;
}

void Nv2aPvpe::Write(uint32_t addr, uint32_t value, unsigned size)
{
   switch (addr)
   {
      default:
         Log(LogLevel::Warning, "Nv2aPvpe: Unimplemented Write %X = %X\n", addr, value); break;
   }
}
