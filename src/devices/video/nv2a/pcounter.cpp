#include "pcounter.h"
#include "nv2a.h"
#include "util.h"
#include <cassert>

Nv2aPcounter::Nv2aPcounter(Nv2a& nv2a) : m_Nv2a(nv2a)
{

}

uint32_t Nv2aPcounter::Read(uint32_t addr, unsigned size)
{
   switch (addr)
   {
      default:
         Log(LogLevel::Warning, "Nv2aPcounter: Unimplemented Read %X\n", addr); break;
   }

   return 0;
}

void Nv2aPcounter::Write(uint32_t addr, uint32_t value, unsigned size)
{
   switch (addr)
   {
      default:
         Log(LogLevel::Warning, "Nv2aPcounter: Unimplemented Write %X = %X\n", addr, value);
         break;
   }
}
