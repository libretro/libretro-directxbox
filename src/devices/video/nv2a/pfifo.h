#ifndef _NV2A_PFIFO_H
#define _NV2A_PFIFO_H

#include <cstdint>
#include "component.h"

class Nv2a;

#define NV_REGION_PFIFO_ADDR    0x00002000
#define NV_REGION_PFIFO_SIZE    0x002000

class Nv2aPfifo : public Nv2aComponent {
public:
    Nv2aPfifo(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);

    uint8_t GetCurrentChannelId();
    uint8_t GetChannelMode(uint8_t channel_id);
private:
    uint32_t m_Regs[NV_REGION_PFIFO_SIZE];
    Nv2a& m_Nv2a;
};

#endif