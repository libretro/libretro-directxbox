#pragma once

#include <cstdint>
#include "pci_bridge.h"

class AGPBridge : public PCIBridge 
{
public:
    // constructor
    AGPBridge();
    virtual ~AGPBridge();

    // PCI Device functions
    void Init() override;
};
