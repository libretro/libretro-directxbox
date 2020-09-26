#pragma once

#include <cstdint>

#include "pci.h"
#include "pci_bus.h"

class PCIBridge : public PCIDevice
{
   public:
      /* constructor */
      PCIBridge();
      virtual ~PCIBridge();

      /* PCI Device functions */
      virtual void Init() override;
      virtual void Reset() override;

      void WriteConfig(uint32_t reg, uint32_t value, uint8_t size) override;

      inline PCIBus *GetSecondaryBus() { return m_secBus; }

   protected:
      PCIBridge(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID);
   private:
      PCIBus *m_secBus;
};
