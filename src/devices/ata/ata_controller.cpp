#include <cstdio>
#include "util.h"
#include "ata_controller.h"

AtaController::AtaController() : 
    PCIDevice(PCI_HEADER_TYPE_NORMAL, PCI_VENDOR_ID_NVIDIA, 0x01BC, 0xD2, 0x01, 0x01, 0x8A) // IDE controller
{
}

AtaController::~AtaController() {
}

// PCI Device functions

void AtaController::Init() {
    RegisterBAR(4, 16, PCI_BAR_TYPE_IO); // 0xFF60 - 0xFF6F

   // Initialize configuration space
    Write16(m_configSpace, PCI_STATUS, PCI_STATUS_FAST_BACK | PCI_STATUS_66MHZ | PCI_STATUS_CAP_LIST);
    Write8(m_configSpace, PCI_CAPABILITY_LIST, 0x44);
    Write8(m_configSpace, PCI_MIN_GNT, 0x03);
    Write8(m_configSpace, PCI_MAX_LAT, 0x01);

    // Capability list
    Write8(m_configSpace, 0x44, PCI_CAP_ID_PM);
    Write8(m_configSpace, 0x45, 0x00);

    // Unknown registers
    Write16(m_configSpace, 0x46, 0x2);
    Write32(m_configSpace, 0x5c, 0xffff00ff);
}

void AtaController::Reset()
{

}

uint32_t AtaController::IORead(int barIndex, uint32_t addr, unsigned size)
{
    Log(LogLevel::Warning, "AtaController: Unimplemented IORead %X\n", addr);
    return 0;
}

void AtaController::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "AtaController:  Unimplemented IOWrite %X\n", addr);
}

uint32_t AtaController::MMIORead(int barIndex, uint32_t addr, unsigned size)
{

    Log(LogLevel::Warning, "AtaController: Unimplemented MMIORead %X\n", addr);
    return 0;
}

void AtaController::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
    Log(LogLevel::Warning, "AtaController: Unimplemented MMIOWrite %X\n", addr);
}

