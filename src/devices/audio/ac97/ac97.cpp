#include "ac97.h"
#include "util.h"

Ac97::Ac97()
    : PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01B1, 0xB1,
        0x04, 0x01, 0x00) // Multimedia Audio Controller
{
}

Ac97::~Ac97() {
}

// PCI Device functions

void Ac97::Init() {
    RegisterBAR(0, 0x100, PCI_BAR_TYPE_IO); // 0xD000 - 0xD0FF
    RegisterBAR(1, 0x80, PCI_BAR_TYPE_IO); // 0xD200 - 0xD27F
    RegisterBAR(2, 0x1000, PCI_BAR_TYPE_MEMORY); // 0xFEC00000 - 0xFEC00FFF

    // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x02);
    Write8(m_configSpace, PCI_MAX_LAT, 0x05);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x2);
    Write16(m_configSpace, 0x4c, 0x106);
}

void Ac97::Reset()
{

}

uint32_t Ac97::IORead(int barIndex, uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "Ac97: Unimplemented IORead %X\n", addr);
    return 0;
}

void Ac97::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "Ac97: Unimplemented IOWrite %X\n", addr);
}

uint32_t Ac97::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "Ac97: Unimplemented MMIORead %X\n", addr);
    return 0;
}

void Ac97::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "Ac97: Unimplemented MMIOWrite %X\n", addr);
}
