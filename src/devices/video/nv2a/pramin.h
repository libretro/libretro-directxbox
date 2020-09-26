#ifndef _NV2A_PRAMIN_H
#define _NV2A_PRAMIN_H

#include <cstdint>
#include "component.h"

class Nv2a;

class Nv2aPramin : public Nv2aComponent {
public:
    Nv2aPramin(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);
private:
    uint8_t m_Data[0x100000];
    Nv2a& m_Nv2a;
};

#endif