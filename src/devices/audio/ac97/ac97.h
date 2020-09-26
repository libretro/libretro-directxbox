#ifndef _AC97_H_
#define _AC97_H_

#include "devices/pci/pci.h"

class Ac97 : public PCIDevice {
    public:
        Ac97();
        ~Ac97();

        void Init();
        void Reset();

        uint32_t IORead(int barIndex, uint32_t addr, unsigned size = sizeof(uint8_t));
        void IOWrite(int barIndex, uint32_t addr, uint32_t data, unsigned size = sizeof(uint8_t));

        uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
        void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);
    private:
};

#endif
