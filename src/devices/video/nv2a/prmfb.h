#ifndef _NV2A_PRMFB_H
#define _NV2A_PRMFB_H

#include <cstdint>
#include "component.h"

class Nv2a;

class Nv2aPrmfb : public Nv2aComponent {
public:
    Nv2aPrmfb(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);
private:
    Nv2a& m_Nv2a;
};

#endif