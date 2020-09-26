#include "xbox.h"
#include <virt86/virt86.hpp>

using namespace virt86;

constexpr int FLASH_REGION_SIZE = 16 * MiB;

Xbox::Xbox(virt86::Platform& platform) : 
    m_HypervisorPlatform(platform), 
    m_I8259(*this),
    m_I8254(*this),
    m_Smbus(*this),
    m_Nv2a(*this)
{

}

bool Xbox::Init()
{
    virt86::VMSpecifications specs = { 0 };
    specs.numProcessors = 1;
    m_Hypervisor = m_HypervisorPlatform.CreateVM(specs);
    if (!m_Hypervisor) {
        Log(LogLevel::Error, "Failed to create Virtual Machine\n");
        return false;
    }

    // Allocate Xbox Memory
    // TODO: Platform independent layer instead of _aligned_malloc
    m_pRam = _aligned_malloc(128 * MiB, PAGE_SIZE);
    if (m_pRam == nullptr) {
        Log(LogLevel::Error, "Failed to allocate Xbox RAM\n");
        return false;
    }

    m_pFlashRegion = _aligned_malloc(FLASH_REGION_SIZE, PAGE_SIZE);
    if (m_pRam == nullptr) {
        Log(LogLevel::Error, "Failed to allocate Flash memory region\n");
        return false;
    }

    auto& hv = m_Hypervisor->get();

    // Map RAM
    auto result = hv.MapGuestMemory(0x00000000, 128 * MiB, virt86::MemoryFlags::Read | virt86::MemoryFlags::Write | virt86::MemoryFlags::Execute, m_pRam);
    if (result != virt86::MemoryMappingStatus::OK) {
        Log(LogLevel::Error, "Failed to map Xbox RAM\n");
        return false;
    }

    result = hv.MapGuestMemory(0xFF000000, FLASH_REGION_SIZE, virt86::MemoryFlags::Read | virt86::MemoryFlags::Execute, m_pFlashRegion);
    if (result != virt86::MemoryMappingStatus::OK) {
        Log(LogLevel::Error, "Failed to map Xbox Flash memory region\n");
        return false;
    }

    hv.RegisterIOReadCallback(IOReadCallback);
    hv.RegisterIOWriteCallback(IOWriteCallback);
    hv.RegisterMMIOReadCallback(MMIOReadCallback);
    hv.RegisterMMIOWriteCallback(MMIOWriteCallback);
    hv.RegisterIOContext(this);

    // Enable Breakpoint, for the cromwell hack
    // This will be automatically disabled at the first breakpoint hit.
    HardwareBreakpoints breakpoint = { 0 };
    breakpoint.bp[0].globalEnable = true;
    breakpoint.bp[0].localEnable = true;
    breakpoint.bp[0].address = 0xFFFFFFFA;
    breakpoint.bp[0].trigger = HardwareBreakpointTrigger::Execution;
    breakpoint.bp[0].length = HardwareBreakpointLength::Byte;
    hv.GetVirtualProcessor(0)->get().SetHardwareBreakpoints(breakpoint);

    // Connect PCI devices to the bus
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(0, 0)), &m_HostBridge);
    //m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(0, 3)), &m_McpxMemoryController);
    //m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 0)), &m_IsaBridge);
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(1, 1)), &m_Smbus);
    //m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(2, 0)), &m_Usb2);
    //m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(3, 0)), &m_Usb1);
    //m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(4, 0)), &m_Nvnet);
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(5, 0)), &m_McpxApu);
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(6, 0)), &m_Ac97);
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(8, 0)), &m_PciBridge);
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(9, 0)), &m_AtaController);
    m_Pci.ConnectDevice(PCI_DEVID(0, PCI_DEVFN(30, 0)),&m_AgpBridge);
    m_Pci.ConnectDevice(PCI_DEVID(1, PCI_DEVFN(0, 0)), &m_Nv2a);

    // Connect SMBUS devices to the bus
    m_Smbus.ConnectDevice(0x10, &m_Smc);
    m_Smbus.ConnectDevice(0x45, &m_ConexantEncoder);
    m_Smbus.ConnectDevice(0x54, &m_Eeprom);
    
    m_Pci.Reset();

    return true; 
}

void Xbox::Shutdown()
{
    // Shutdown the VM
    if (m_Hypervisor) {
        m_HypervisorPlatform.FreeVM(m_Hypervisor->get());
    }

    // Free memory
    if (m_pRam != nullptr) {
        _aligned_free(m_pRam);
        m_pRam = nullptr;
    }

    if (m_pFlashRegion != nullptr) {
        _aligned_free(m_pFlashRegion);
        m_pFlashRegion = nullptr;
    }

    if (m_pFlashRom != nullptr) {
        free(m_pFlashRom);
        m_pFlashRom = nullptr;
    }
}

void Xbox::Reset()
{

}

bool Xbox::LoadBootRoms(std::string flash, std::string mcpx)
{
    // Load Flash
    FILE* fp = fopen(flash.c_str(), "rb");
    if (fp == nullptr) {
        Log(LogLevel::Error, "Failed to open flash rom: %s\n", flash.c_str());
        return false;
    }

    // Get bios file size
    fseek(fp, 0, SEEK_END);
    long biosSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Check the bios for validity (must be a multiple of 0x10000)
    if (biosSize % 0x10000 != 0) {
        Log(LogLevel::Error, "Flash rom (%s) has an invalid size\n", flash.c_str());
        return 0;
    }

    // Allocate memory for bios
    m_pFlashRom = malloc(biosSize);
    if (m_pFlashRom == nullptr) {
        Log(LogLevel::Error, "Failed to allocate memory for flash rom\n");
        return false;
    }

    // Load the bios into memory
    fread(m_pFlashRom, 1, biosSize, fp);
    fclose(fp);

    // Map the bios at the top of the memory, mirrored from 0xFF000000 to end of memory
    memcpy(m_pFlashRegion, m_pFlashRom, biosSize);
    for (uint32_t addr = biosSize; addr < (16 * MiB); addr += biosSize) {
        memcpy((uint8_t*)m_pFlashRegion + addr, m_pFlashRom, biosSize);
    }

    // If we were not given an MCPX rom as input, we are done here
    // This is to support systems that don't have a secret rom (Debug/Chihiro)
    if (mcpx.length() == 0) {
        return true;
    }

    // Load MCPX rom
    fp = fopen(mcpx.c_str(), "rb");
    if (fp == nullptr) {
        Log(LogLevel::Error, "Failed to open mcpx rom: %s\n", mcpx.c_str());
        return false;
    }

    // Get MCPX file size
    fseek(fp, 0, SEEK_END);
    long mcpxSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Check the bios for validity (must be a multiple of 0x10000)
    if (mcpxSize != 512) {
        Log(LogLevel::Error, "Mcpx rom (%s) has an invalid size\n", mcpx.c_str());
        fclose(fp);
        return false;
    }

    // Load the mcpx into memory, overlays the last 512 bytes of address space
    fread((uint8_t*)m_pFlashRegion + FLASH_REGION_SIZE - 512, 1, mcpxSize, fp);
    fclose(fp);

    return true;
}

I8259& Xbox::GetInterruptController()
{
    return m_I8259;
}

void Xbox::Interrupt(int irq)
{
    auto& hv = m_Hypervisor->get();
    auto& vp = hv.GetVirtualProcessor(0)->get();

    if (irq != 48 && irq != 63) {
        Log(LogLevel::Info, "Queing Interrupt: %d\n", irq);
    }
    vp.EnqueueInterrupt(irq);
}

void Xbox::RunFrame()
{
    static auto& hv = m_Hypervisor->get();
    static auto& vp = hv.GetVirtualProcessor(0)->get();

    // HACK: Make sure we run long enough for 1 frame duration
    auto framestart = std::chrono::high_resolution_clock::now();
    auto frameEnd = framestart + std::chrono::milliseconds(16);
    while (std::chrono::high_resolution_clock::now() < frameEnd) {
        auto result = vp.Run();
        if (result != virt86::VPExecutionStatus::OK) {
            // TODO: Log error and shutdown
            Log(LogLevel::Error, "Error occured");
            return;
        }

        auto& exitInfo = vp.GetVMExitInfo();
        switch (exitInfo.reason) {
            case VMExitReason::Shutdown:
            case VMExitReason::HLT: {
                Log(LogLevel::Info, "System Shutdown!\n");
            } break;
            case VMExitReason::Error:
                // TODO: Log error and shutdown
                Log(LogLevel::Error, "Error occured");
                return;
            case VMExitReason::HardwareBreakpoint: {
                RegValue eip;
                vp.RegRead(Reg::EIP, eip);
                // This was most likely a breakpoint from mcpx for the cromwell hack
                // If so, we can disable breakpoints and force eip to 0xFFFC100B.
                // This forces cromwell to skip a problematic MSR write + prevent EIP overflow
                if (eip.u32 == 0xFFFFFFFA) {
                    Log(LogLevel::Info, "Applying Cromwell boot hack\n");
                    eip.u32 = 0xFFFC100B;
                    vp.RegWrite(Reg::EIP, eip);
                }
                vp.ClearHardwareBreakpoints();
            } break;
        }
    }
}

uint8_t* Xbox::GetRamPtr()
{
    return (uint8_t*)m_pRam;
}

Nv2a& Xbox::GetNv2a()
{
    return m_Nv2a;
}

uint32_t Xbox::IOReadCallback(void *context, uint16_t port, size_t size)
{
    Xbox *xbox = (Xbox *)context;
    return xbox->IORead(port, size);
}

void Xbox::IOWriteCallback(void *context, uint16_t port, size_t size, uint32_t value) 
{
    Xbox *xbox = (Xbox *)context;
    xbox->IOWrite(port, size, value);
}
    
uint64_t Xbox::MMIOReadCallback(void *context, uint64_t address, size_t size) {
    Xbox *xbox = (Xbox *)context;
    return xbox->MMIORead(address, size);
}

void Xbox::MMIOWriteCallback(void *context, uint64_t address, size_t size, uint64_t value) {
    Xbox *xbox = (Xbox *)context;
    xbox->MMIOWrite(address, size, value);
}

#include <Windows.h>

uint32_t Xbox::IORead(uint16_t port, size_t size) 
{
    // First, attempt to handle static IO Mapped Devices
    switch (port) {
        case 0x8008: { // TODO : Move 0x8008 TIMER to a device
            if (size == sizeof(uint32_t)) {
                // HACK: This is very wrong.
                // This timer should count at a specific frequency (3579.545 ticks per ms)
                // But this is enough to keep NXDK from hanging for now.
                LARGE_INTEGER performanceCount;
                QueryPerformanceCounter(&performanceCount);
                return static_cast<uint32_t>(performanceCount.QuadPart);
            }
            break;
        }
        case PORT_PIC_MASTER_COMMAND:
        case PORT_PIC_MASTER_DATA:
        case PORT_PIC_SLAVE_COMMAND:
        case PORT_PIC_SLAVE_DATA:
        case PORT_PIC_MASTER_ELCR:
        case PORT_PIC_SLAVE_ELCR:
            return m_I8259.IORead(port);
        case PORT_PIT_DATA_0:
        case PORT_PIT_DATA_1:
        case PORT_PIT_DATA_2:
        case PORT_PIT_COMMAND:
            return m_I8254.IORead(port);
    }

    // Map VGA register to NV2A PRMCIO
    if (port >= 0x3C0 && port <= 0x3DF) {
        return m_Nv2a.Prmcio.Read(port, size);
    }

    // Next, PCI
    uint32_t value = 0;
    if (!m_Pci.IORead(port, &value, size)) {
        Log(LogLevel::Warning, "Unhandled IO Read (Port = %04X)\n", port);
    }

    return value;
}

void Xbox::IOWrite(uint16_t port, size_t size, uint32_t value) 
{
    // First attempt to handle static IO Mapped Devices
    switch (port) {
        case PORT_PIC_MASTER_COMMAND:
        case PORT_PIC_MASTER_DATA:
        case PORT_PIC_SLAVE_COMMAND:
        case PORT_PIC_SLAVE_DATA:
        case PORT_PIC_MASTER_ELCR:
        case PORT_PIC_SLAVE_ELCR:
            m_I8259.IOWrite(port, value);
            return;
        case PORT_PIT_DATA_0:
        case PORT_PIT_DATA_1:
        case PORT_PIT_DATA_2:
        case PORT_PIT_COMMAND:
            m_I8254.IOWrite(port, value);
            return;
    }

    // Map VGA register to NV2A PRMCIO
    if (port >= 0x3C0 && port <= 0x3DF) {
        m_Nv2a.Prmcio.Write(port, value, size);
        return;
    }

    // Next, PCI
    if (!m_Pci.IOWrite(port, value, size)) {
        Log(LogLevel::Warning, "Unhandled IO Write (Port = %04X, Value = %08X)\n", port, value);
    }
}

uint64_t Xbox::MMIORead(uint64_t address, size_t size) 
{
    uint32_t value = 0;
    if (!m_Pci.MMIORead(address, &value, size)) {
        Log(LogLevel::Warning, "Unhandled MMIO Read (Addr = %08X)\n", (uint32_t)address);
    }
    return value;
}

void Xbox::MMIOWrite(uint64_t address, size_t size, uint64_t value) 
{
    if (!m_Pci.MMIOWrite(address, value, size)) {
        Log(LogLevel::Warning, "Unhandled MMIO Write (Addr = %08X, Value = %08X)\n", address, (uint32_t)value);
    }
}
