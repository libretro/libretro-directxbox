#include "nv2a.h"
#include "util.h"
#include "xbox.h"

#include <cassert>

Nv2a::Nv2a(Xbox& xbox) : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x02A0, 0xA1,
    0x03, 0x00, 0x00), /* VGA-compatible controller */
    m_Xbox(xbox),
    Pmc(*this),
    Pbus(*this),
    Pfifo(*this),
    Prma(*this),
    Pvideo(*this),
    Ptimer(*this),
    Pcounter(*this),
    Pvpe(*this),
    Ptv(*this),
    Prmfb(*this),
    Prmvio(*this),
    Pfb(*this),
    Pstraps(*this),
    Pgraph(*this),
    Pcrtc(*this),
    Prmcio(*this),
    Pramdac(*this),
    Prmdio(*this),
    Pramin(*this),
    User(*this)
{
   m_MemoryRegions.push_back({ NV_REGION_PMC_ADDR, NV_REGION_PMC_SIZE, Pmc });
   m_MemoryRegions.push_back({ NV_REGION_PBUS_ADDR, NV_REGION_PBUS_SIZE, Pbus });
   m_MemoryRegions.push_back({ NV_REGION_PFIFO_ADDR, NV_REGION_PFIFO_SIZE, Pfifo });
   m_MemoryRegions.push_back({ NV_REGION_PRMA_ADDR, NV_REGION_PRMA_SIZE, Prma }); 
   m_MemoryRegions.push_back({ NV_REGION_PVIDEO_ADDR, NV_REGION_PVIDEO_SIZE, Pvideo });
   m_MemoryRegions.push_back({ NV_REGION_PTIMER_ADDR, NV_REGION_PTIMER_SIZE, Ptimer });
   m_MemoryRegions.push_back({ NV_REGION_PCOUNTER_ADDR, NV_REGION_PCOUNTER_SIZE, Pcounter });
   m_MemoryRegions.push_back({ NV_REGION_PVPE_ADDR, NV_REGION_PVPE_SIZE, Pvpe });
   m_MemoryRegions.push_back({ NV_REGION_PTV_ADDR, NV_REGION_PTV_SIZE, Ptv });
   m_MemoryRegions.push_back({ NV_REGION_PRMFB_ADDR, NV_REGION_PRMFB_SIZE, Prmfb });
   m_MemoryRegions.push_back({ NV_REGION_PRMVIO_ADDR, NV_REGION_PRMVIO_SIZE, Prmvio });
   m_MemoryRegions.push_back({ NV_REGION_PFB_ADDR, NV_REGION_PFB_SIZE, Pfb });
   m_MemoryRegions.push_back({ NV_REGION_PSTRAPS_ADDR, NV_REGION_PSTRAPS_SIZE, Pstraps });
   m_MemoryRegions.push_back({ NV_REGION_PGRAPH_ADDR, NV_REGION_PGRAPH_SIZE, Pstraps });
   m_MemoryRegions.push_back({ NV_REGION_PCRTC_ADDR, NV_REGION_PCRTC_SIZE, Pcrtc });
   m_MemoryRegions.push_back({ NV_REGION_PRMCIO_ADDR, NV_REGION_PRMCIO_SIZE, Prmcio });
   m_MemoryRegions.push_back({ NV_REGION_PRAMDAC_ADDR, NV_REGION_PRAMDAC_SIZE, Pramdac });
   m_MemoryRegions.push_back({ NV_REGION_PRMDIO_ADDR, NV_REGION_PRMDIO_SIZE, Prmdio });
   m_MemoryRegions.push_back({ NV_REGION_PRAMIN_ADDR, NV_REGION_PRAMIN_SIZE, Pramin });
   m_MemoryRegions.push_back({ NV_REGION_USER_ADDR, NV_REGION_USER_SIZE, User });

   m_pRam = nullptr;
}

Nv2a::~Nv2a()
{

}

void Nv2a::Init()
{
   m_pRam = m_Xbox.GetRamPtr();
   RegisterBAR(0, 0x1000000, PCI_BAR_TYPE_MEMORY); /* 0xFD000000 - 0xFDFFFFFF */
   RegisterBAR(1, 0x8000000, PCI_BAR_TYPE_MEMORY | PCI_BAR_MEMORY_PREFETCHABLE); /* 0xF0000000 - 0xF7FFFFFF */
   RegisterBAR(2, 0x80000, PCI_BAR_TYPE_MEMORY | PCI_BAR_MEMORY_PREFETCHABLE); /* 0x0 - 0x7FFFF */

   Write8(m_configSpace, PCI_LATENCY_TIMER, 0xf8);

   /* Initialize configuration space */
   Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST | PCI_STATUS_DEVSEL_MEDIUM);
   Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x60);
   Write8(m_configSpace, PCI_MIN_GNT, 0x05);
   Write8(m_configSpace, PCI_MAX_LAT, 0x01);

   /* Capability list */
   Write8(m_configSpace, 0x60, PCI_CAP_ID_PM);
   Write8(m_configSpace, 0x61, 0x44);

   Write8(m_configSpace, 0x44, PCI_CAP_ID_AGP);
   Write8(m_configSpace, 0x45, 0x00);

   /* Unknown registers */
   Write16(m_configSpace, 0x46, 0x20);
   Write32(m_configSpace, 0x48, 0x1f000017);
   Write16(m_configSpace, 0x4c, 0x1f00);
   Write32(m_configSpace, 0x54, 0x1);
   Write32(m_configSpace, 0x58, 0x23d6ce);
   Write32(m_configSpace, 0x5c, 0xf);
   Write32(m_configSpace, 0x60, 0x24401);
   Write32(m_configSpace, 0x80, 0x2b16d065);
}

void Nv2a::Reset()
{

}

uint32_t Nv2a::IORead(int barIndex, uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "Nv2a: Unimplemented IORead %X\n", addr);
    return 0;
}

void Nv2a::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "Nv2a: Unimplemented IOWrite %X\n", addr);
}

uint32_t Nv2a::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
   if (barIndex == 1)
   {
      switch (size)
      {
         case sizeof(uint8_t): return m_pRam[addr];
         case sizeof(uint16_t): return *(uint16_t*)&m_pRam[addr];
         case sizeof(uint32_t): return *(uint32_t*)&m_pRam[addr];
      }
   }

   if (barIndex == 0)
   {
      auto region = GetMemoryRegion(addr);
      if (region)
         return region->component.Read(addr - region->addr, size);
   }

   Log(LogLevel::Warning, "Nv2a: Unimplemented MMIORead %X\n", addr);
   return 0;
}

void Nv2a::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
   if (barIndex == 1)
   {
      switch (size)
      {
         case sizeof(uint8_t): m_pRam[addr] = value; return;
         case sizeof(uint16_t): *(uint16_t*)&m_pRam[addr] = value; return;
         case sizeof(uint32_t): *(uint32_t*)&m_pRam[addr] = value; return;
      }
   }

   if (barIndex == 0)
   {
      auto region = GetMemoryRegion(addr);
      if (region)
      {
         region->component.Write(addr - region->addr, value, size);
         return;
      }
   }

   Log(LogLevel::Warning, "Nv2a: Unimplemented MMIOWrite %X\n", addr);
}

void Nv2a::UpdateIrq()
{
   /* TODO: Check for interrupts */
}

Nv2aRegion* Nv2a::GetMemoryRegion(uint32_t addr)
{
   for(auto it = m_MemoryRegions.begin(); it != m_MemoryRegions.end(); ++it)
   {
      auto& region = *it;
      if (addr >= region.addr && addr < region.addr + region.size)
         return &region;
   }

    Log(LogLevel::Warning, "Unknown NV2A access (addr = %X)", addr);
    return nullptr;
}
