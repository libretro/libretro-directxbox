#pragma once

#include "pci.h"
#include <map>

#define PORT_PCI_CONFIG_ADDRESS   0xCF8
#define PORT_PCI_CONFIG_DATA      0xCFC
#define PCI_CONFIG_REGISTER_MASK  0xFC

class PCIBus 
{
   public:
      PCIBus();
      virtual ~PCIBus();

      void ConnectDevice(uint32_t deviceId, PCIDevice *pDevice);

      bool IORead(uint32_t port, uint32_t *value, unsigned size);
      bool IOWrite(uint32_t port, uint32_t value, unsigned size);

      bool MMIORead(uint32_t addr, uint32_t *value, unsigned size);
      bool MMIOWrite(uint32_t addr, uint32_t value, unsigned size);

      void Reset();
   private:
      friend class PCIDevice;
      friend class PCIBridge;

      PCIDevice *m_owner; /* The bridge that owns this bus */
      std::map<uint32_t, PCIDevice*> m_Devices;
      PCIConfigAddressRegister m_configAddressRegister;

      void IOWriteConfigAddress(uint32_t pData);
      void IOWriteConfigData(uint32_t pData, uint8_t size, uint8_t regOffset);
      uint32_t IOReadConfigData(uint8_t size, uint8_t regOffset);
};
