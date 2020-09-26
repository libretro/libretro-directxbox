#pragma once

#include <cstdint>

#include "pci.h"

class HostBridge : public PCIDevice {
public:
    // constructor
    HostBridge();
     ~HostBridge();

    // PCI Device functions
    void Init();
    void Reset();

    uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size) override;
    void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) override;
};
