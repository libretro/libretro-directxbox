#ifndef _NV2A_PRMCIO_H
#define _NV2A_PRMCIO_H

#include <cstdint>
#include "component.h"

class Nv2a;

#define NV_REGION_PRMCIO_ADDR   0x00601000
#define NV_REGION_PRMCIO_SIZE   0x001000

class Nv2aPrmcio : public Nv2aComponent {
public:
    Nv2aPrmcio(Nv2a& nv2a);
    uint32_t Read(uint32_t addr, unsigned size);
    void Write(uint32_t addr, uint32_t value, unsigned size);

    uint8_t GetDisplayBpp();
    uint16_t GetDisplayWidth();
    uint16_t GetDisplayHeight();
    // TODO: GetDisplayFormat
private:
    Nv2a& m_Nv2a;
    uint16_t m_Index;
    uint32_t m_Regs[NV_REGION_PRMCIO_SIZE];
};

#endif