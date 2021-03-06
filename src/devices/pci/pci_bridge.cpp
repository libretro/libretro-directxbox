#include "pci_bridge.h"
#include "util.h"

PCIBridge::PCIBridge() : PCIBridge(PCI_VENDOR_ID_NVIDIA, 0x01B8, 0xB1)
{
}

PCIBridge::PCIBridge(uint16_t vendorID, uint16_t deviceID, uint8_t revisionID)
    : PCIDevice(PCI_HEADER_TYPE_BRIDGE, vendorID, deviceID, revisionID,
        0x06, 0x04, 0x00, /* PCI bridge */
        /*TODO: subsystemVendorID*/ 0x00,
        /*TODO: subsystemID*/       0x00)
{
    m_secBus = new PCIBus();
}

PCIBridge::~PCIBridge()
{
    delete m_secBus;
}

/* PCI Device functions */

void PCIBridge::Init()
{
   /* Initialize configuration space */
   TestAndSet16(m_configSpace, PCI_STATUS, PCI_STATUS_66MHZ | PCI_STATUS_FAST_BACK);
   Write16(m_configSpace, PCI_SEC_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_DEVSEL_MEDIUM);
   Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);

   /* Capabilities list */
   Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
   Write8(m_configSpace, 0x45, 0x0);

   /* Unknown registers */
   Write16(m_configSpace, 0x46, 0x2);
   Write16(m_configSpace, 0x4c, 0xb08);
   Write16(m_configSpace, 0x50, 0xd0c);
   Write16(m_configSpace, 0x54, 0xf0e);

   m_secBus->m_owner = this;
}

void PCIBridge::Reset() {
    Write8(m_configSpace, PCI_PRIMARY_BUS, 0);
    Write8(m_configSpace, PCI_SECONDARY_BUS,  0);
    Write8(m_configSpace, PCI_SUBORDINATE_BUS, 0);
    Write8(m_configSpace, PCI_SEC_LATENCY_TIMER, 0);

    TestAndSet8(m_configSpace, PCI_IO_BASE, PCI_IO_RANGE_MASK & 0xff);
    TestAndSet8(m_configSpace, PCI_IO_LIMIT, PCI_IO_RANGE_MASK & 0xff);
    TestAndSet16(m_configSpace, PCI_MEMORY_BASE, PCI_MEMORY_RANGE_MASK & 0xffff);
    TestAndSet16(m_configSpace, PCI_MEMORY_LIMIT, PCI_MEMORY_RANGE_MASK & 0xffff);
    TestAndSet16(m_configSpace, PCI_PREF_MEMORY_BASE, PCI_PREF_RANGE_MASK & 0xffff);
    TestAndSet16(m_configSpace, PCI_PREF_MEMORY_LIMIT, PCI_PREF_RANGE_MASK & 0xffff);
    Write32(m_configSpace, PCI_PREF_BASE_UPPER32, 0);
    Write32(m_configSpace, PCI_PREF_LIMIT_UPPER32, 0);

    Write16(m_configSpace, PCI_BRIDGE_CONTROL, 0);

    m_secBus->Reset();
}

void PCIBridge::WriteConfig(uint32_t reg, uint32_t value, uint8_t size)
{
   uint16_t oldControl = Read16(m_configSpace, PCI_BRIDGE_CONTROL);
   uint16_t newControl;

   PCIDevice::WriteConfig(reg, value, size);

   if (RangesOverlap(reg, size, PCI_COMMAND, 2) ||

         /* I/O base/limit */
         RangesOverlap(reg, size, PCI_IO_BASE, 2) ||

         /* Memory base/limit, prefetchable base/limit 
          * and I/O base/limit upper 16 */
         RangesOverlap(reg, size, PCI_MEMORY_BASE, 20) ||

         /* VGA enable */
         RangesOverlap(reg, size, PCI_BRIDGE_CONTROL, 2)) {

      /* TODO: update mappings */
#if 0
      UpdateIOMappings();
#endif
   }

   newControl = Read16(m_configSpace, PCI_BRIDGE_CONTROL);
   /* Trigger hot reset on 0->1 transition */
   if (~oldControl & newControl & PCI_BRIDGE_CTL_BUS_RESET)
      m_secBus->Reset();
}
