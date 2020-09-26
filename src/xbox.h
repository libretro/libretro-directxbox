#pragma once

#include <string>
#include <virt86/virt86.hpp>

#include "devices/pci/pci_bus.h"
#include "devices/pci/agp_bridge.h"
#include "devices/pci/host_bridge.h"
#include "devices/pci/pci_bridge.h"
#include "devices/ata/ata_controller.h"
#include "devices/smbus.h"
#include "devices/smc.h"
#include "devices/audio/ac97/ac97.h"
#include "devices/audio/mcpx/mcpx_apu.h"
#include "devices/video/tv/conexant.h"
#include "devices/video/nv2a/nv2a.h"
#include "devices/i8259.h"
#include "devices/i8254.h"
#include "devices/eeprom.h"

#include "util.h"

class Xbox
{
public:
    Xbox(virt86::Platform& platform);
    bool Init();
    void Shutdown();
    bool LoadBootRoms(std::string flash, std::string mcpx = "");

    I8259& GetInterruptController();
    void Interrupt(int irq);

    void Reset();
    void RunFrame();

    uint8_t* GetRamPtr();

    Nv2a& GetNv2a();
private:
    // Memory
    void* m_pRam = nullptr;
    void* m_pFlashRegion = nullptr;
    void* m_pFlashRom = nullptr;
    void* m_pMcpxRom = nullptr;

    // Bus Devices
    PCIBus m_Pci;
    SMBus m_Smbus;

    // PCI Devices
    HostBridge m_HostBridge;
    PCIBridge m_PciBridge;
    AGPBridge m_AgpBridge;
    
    AtaController m_AtaController;
    Ac97 m_Ac97;
    McpxApu m_McpxApu;
    Nv2a m_Nv2a;

    // SMBus Devices
    SMC m_Smc;
    ConexantTVEncoder m_ConexantEncoder;
    Eeprom m_Eeprom;

    // ISA Devices
    I8259 m_I8259;
    I8254 m_I8254;

    // Hypervisor Platform
    virt86::Platform& m_HypervisorPlatform;
    std::optional<std::reference_wrapper<virt86::VirtualMachine>> m_Hypervisor;

    // IO Callbacks
    static uint32_t IOReadCallback(void *context, uint16_t port, size_t size);
    static void IOWriteCallback(void *context, uint16_t port, size_t size, uint32_t value);
    static uint64_t MMIOReadCallback(void *context, uint64_t address, size_t size);
    static void MMIOWriteCallback(void *context, uint64_t address, size_t size, uint64_t value);

    // IO Implementations
    uint32_t IORead(uint16_t port, size_t size);
    void IOWrite(uint16_t port, size_t size, uint32_t value);
    uint64_t MMIORead(uint64_t address, size_t size);
    void MMIOWrite(uint64_t address, size_t size, uint64_t value);
};
