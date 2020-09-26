#ifndef _NV2A_PCRTC_H
#define _NV2A_PCRTC_H

#include <cstdint>
#include "component.h"

class Nv2a;

#define NV_REGION_PCRTC_ADDR    0x00600000
#define NV_REGION_PCRTC_SIZE    0x001000

class Nv2aPcrtc : public Nv2aComponent {
public:
    Nv2aPcrtc(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);

    uint32_t GetFramebufferAddr();
private:
    uint32_t m_Regs[NV_REGION_PCRTC_SIZE];
    Nv2a& m_Nv2a;
};

#endif