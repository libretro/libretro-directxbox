#ifndef _ATA_H_
#define _ATA_H_

#include "devices/pci/pci.h"

class AtaController : public PCIDevice {
public:
    AtaController();
    ~AtaController();

    // PCI Functions
    void Init();
    void Reset();

    uint32_t IORead(int barIndex, uint32_t addr, unsigned size = sizeof(uint8_t));
    void IOWrite(int barIndex, uint32_t addr, uint32_t data, unsigned size = sizeof(uint8_t));

    uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
    void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
private:

};

#endif
