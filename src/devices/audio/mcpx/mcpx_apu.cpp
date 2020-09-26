#include <cstdio>
#include "util.h"
#include "mcpx_apu.h"

/* TODO: Everything */

#define APU_VP_BASE 0x20000
#define APU_VP_SIZE 0x10000

#define APU_GP_BASE 0x30000
#define APU_GP_SIZE 0x10000

#define APU_EP_BASE 0x50000
#define APU_EP_SIZE 0x10000

McpxApu::McpxApu()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B0, 0xB1,
        0x04, 0x01, 0x00) /* Multimedia Audio Controller */
{
}

McpxApu::~McpxApu() {
}

/* PCI Device functions */

void McpxApu::Init() {
    RegisterBAR(0, 0x80000, PCI_BAR_TYPE_MEMORY); /* 0xFE800000 - 0xFE87FFFF */

    /* Initialize configuration space */
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x01);
    Write8(m_configSpace, PCI_MAX_LAT, 0x0c);

    /* Capability list */
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    /* Unknown registers */
    Write16(m_configSpace, 0x46, 0x2);
    Write32(m_configSpace, 0x4c, 0x50a);
    for (uint8_t i = 0; i < 0x100 - 0x50; i += 4) {
        Write32(m_configSpace, 0x50 + i, 0x20001);
    }
}

void McpxApu::Reset()
{

}

uint32_t McpxApu::IORead(int barIndex, uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented IORead %X\n", addr);
    return 0;
}

void McpxApu::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented IOWrite %X\n", addr);
}

uint32_t McpxApu::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
    if (addr >= APU_VP_BASE && addr < APU_VP_BASE + APU_VP_SIZE) {
        return VPRead(addr - APU_VP_BASE, size);
    }

    if (addr >= APU_GP_BASE && addr < APU_GP_BASE + APU_GP_SIZE) {
        return GPRead(addr - APU_GP_BASE, size);
    }

    if (addr >= APU_EP_BASE && addr < APU_EP_BASE + APU_EP_SIZE) {
        return EPRead(addr - APU_EP_BASE, size);
    }

    Log(LogLevel::Warning, "McpxApu:  Unimplemented MMIORead %X\n", addr);
    return 0;
}

void McpxApu::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    if (addr >= APU_VP_BASE && addr < APU_VP_BASE + APU_VP_SIZE) {
        VPWrite(addr - APU_VP_BASE, value, size);
        return;
    }

    if (addr >= APU_GP_BASE && addr < APU_GP_BASE + APU_GP_SIZE) {
        GPWrite(addr - APU_GP_BASE, value, size);
        return;
    }

    if (addr >= APU_EP_BASE && addr < APU_EP_BASE + APU_EP_SIZE) {
        EPWrite(addr - APU_EP_BASE, value, size);
        return;
    }

    Log(LogLevel::Warning, "McpxApu:  Unimplemented MMIOWrite %X\n", addr);
}


uint32_t McpxApu::GPRead(uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented GP MMIORead %X\n", addr);
    return 0;
}

void McpxApu::GPWrite(uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented GP MMIOWrite %X\n", addr);
}


uint32_t McpxApu::VPRead(uint32_t addr, unsigned size)
{
    switch (addr)
    {
       case 0x10:
          return 0x80; /* HACK: Pretend the FIFO is always empty, bypasses hangs when APU isn't fully implemented */
    }

    Log(LogLevel::Warning, "McpxApu:  Unimplemented VP MMIORead %X\n", addr);
    return 0;
}

void McpxApu::VPWrite(uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented VP MMIOWrite %X\n", addr);
}


uint32_t McpxApu::EPRead(uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented EP MMIORead %X\n", addr);
    return 0;
}

void McpxApu::EPWrite(uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "McpxApu:  Unimplemented EP MMIOWrite %X\n", addr);
}
