#ifndef _NV2A_COMPONENT_H
#define _NV2A_COMPONENT_H

#include <cstdint>
class Nv2a;

class Nv2aComponent
{
   public:
      virtual uint32_t Read(uint32_t addr, unsigned size) = 0;
      virtual void Write(uint32_t addr, uint32_t value, unsigned size) = 0;
};

#endif
