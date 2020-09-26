include_directories(.)

set(XBOX_SOURCE
    libretro/libretro.cpp
    xbox.cpp
    xbox_database.cpp

    devices/pci/agp_bridge.cpp
    devices/pci/host_bridge.cpp
    devices/pci/pci.cpp
    devices/pci/pci_bridge.cpp
    devices/pci/pci_bus.cpp

    devices/ata/ata_controller.cpp
    devices/smbus.cpp
    devices/smc.cpp
    devices/i8259.cpp
    devices/i8254.cpp
    devices/eeprom.cpp

    devices/audio/ac97/ac97.cpp
    devices/audio/mcpx/mcpx_apu.cpp

    devices/video/nv2a/nv2a.cpp
    devices/video/nv2a/pmc.cpp
    devices/video/nv2a/pbus.cpp
    devices/video/nv2a/pfifo.cpp
    devices/video/nv2a/prma.cpp
    devices/video/nv2a/pvideo.cpp
    devices/video/nv2a/ptimer.cpp
    devices/video/nv2a/pcounter.cpp
    devices/video/nv2a/pvpe.cpp
    devices/video/nv2a/ptv.cpp
    devices/video/nv2a/prmfb.cpp
    devices/video/nv2a/prmvio.cpp
    devices/video/nv2a/pfb.cpp
    devices/video/nv2a/pstraps.cpp
    devices/video/nv2a/pgraph.cpp
    devices/video/nv2a/pcrtc.cpp
    devices/video/nv2a/prmcio.cpp
    devices/video/nv2a/pramdac.cpp
    devices/video/nv2a/prmdio.cpp
    devices/video/nv2a/pramin.cpp
    devices/video/nv2a/user.cpp

    devices/video/tv/conexant.cpp
)

find_package(virt86 CONFIG REQUIRED)
add_library(libretro-xbox SHARED ${XBOX_SOURCE})
target_link_libraries(libretro-xbox PUBLIC virt86::virt86)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Ot /Oi /arch:AVX /sdl- /GL") 
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")        ## Optimize
endif()

