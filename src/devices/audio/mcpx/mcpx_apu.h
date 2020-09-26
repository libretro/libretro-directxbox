#ifndef _APU_H_
#define _APU_H_

#include "devices/pci/pci.h"

class McpxApu : public PCIDevice {
public:
    McpxApu();
    ~McpxApu();

    // PCI Functions
    void Init();
    void Reset();

    uint32_t IORead(int barIndex, uint32_t addr, unsigned size = sizeof(uint8_t));
    void IOWrite(int barIndex, uint32_t addr, uint32_t data, unsigned size = sizeof(uint8_t));

    uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
    void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
private:
    // TODO: Split into child devices
    uint32_t GPRead(uint32_t addr, unsigned size);
    void GPWrite(uint32_t addr, uint32_t value, unsigned size);
    uint32_t EPRead(uint32_t addr, unsigned size);
    void EPWrite(uint32_t addr, uint32_t value, unsigned size);
    uint32_t VPRead(uint32_t addr, unsigned size);
    void VPWrite(uint32_t addr, uint32_t value, unsigned size);
};

#endif
