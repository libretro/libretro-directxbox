#ifndef _NV2A_PMC_H
#define _NV2A_PMC_H

#include <cstdint>
#include "component.h"

class Nv2a;

class Nv2aPmc : public Nv2aComponent {
public:
    Nv2aPmc(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);
private:
    Nv2a& m_Nv2a;
};

#endif