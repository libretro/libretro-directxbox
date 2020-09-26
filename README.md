A work-in-progress XBox OG emulator, written from the ground up for Libretro.

Currently requires the following BIOS images to be placed in the System directory:
* mcpx_1.0.bin
* cromwell_1024.bin

Requirements/dependencies:
* Libvirt86

Status:
+ PCI/ISA/SMBUS devices are partially implemented
+ Enough of the GPU is implemented for direct framebuffer access to work 
- Microsoft BIOS doesn't boot yet because ATA & OCHI are missing
- No hardware acceleration support yet (software rendering for now)

Compatibility:
Not much for now. Cromwell BIOS can boot, homebrew that theoretically only relies on basic framebuffer mode could theoretically work already.

Cromwell BIOS can only boot images, but not games.

Microsoft BIOS doesn't work yet (too much hardware that isn't implemented yet like ATA & OCHI)

