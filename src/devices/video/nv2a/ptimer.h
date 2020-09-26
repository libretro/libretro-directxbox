#ifndef _NV2A_PTIMER_H
#define _NV2A_PTIMER_H

#include <cstdint>
#include "component.h"

class Nv2a;

class Nv2aPtimer : public Nv2aComponent {
public:
    Nv2aPtimer(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);
private:
    Nv2a& m_Nv2a;
};

#endif