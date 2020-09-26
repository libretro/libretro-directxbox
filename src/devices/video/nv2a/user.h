#ifndef _NV2A_USER_H
#define _NV2A_USER_H

#include <cstdint>
#include "component.h"

class Nv2a;

#define NV_REGION_USER_ADDR     0x00800000
#define NV_REGION_USER_SIZE     0x800000

class Nv2aUser : public Nv2aComponent {
public:
    Nv2aUser(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);
private:
    uint32_t m_Regs[NV_REGION_USER_SIZE];
    Nv2a& m_Nv2a;
};

#endif