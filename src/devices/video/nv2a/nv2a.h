#ifndef _NV2A_H_
#define _NV2A_H_

class Xbox;

#include "devices/pci/pci.h"
#include "regs.h"

#include "component.h"
#include "pmc.h"
#include "pbus.h"
#include "pfifo.h"
#include "prma.h"
#include "pvideo.h"
#include "ptimer.h"
#include "pcounter.h"
#include "pvpe.h"
#include "ptv.h"
#include "prmfb.h"
#include "prmvio.h"
#include "pfb.h"
#include "pstraps.h"
#include "pgraph.h"
#include "pcrtc.h"
#include "prmcio.h"
#include "pramdac.h"
#include "prmdio.h"
#include "pramin.h"
#include "user.h"

#include <vector>

struct Nv2aRegion
{
    uint32_t addr;
    uint32_t size;
    Nv2aComponent& component;
};

typedef struct {
    uint8_t fb_start;
    uint8_t width;
    uint8_t height;
    uint8_t pitch;
} DisplayInfo;

class Nv2a : public PCIDevice {
    public:
        Nv2a(Xbox& xbox);
        virtual ~Nv2a();

        // PCI Functions
        void Init();
        void Reset();

        uint32_t IORead(int barIndex, uint32_t addr, unsigned size);
        void IOWrite(int barIndex, uint32_t addr, uint32_t data, unsigned size);
        uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
        void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);

        Nv2aPmc Pmc;
        Nv2aPbus Pbus;
        Nv2aPfifo Pfifo;
        Nv2aPrma Prma;
        Nv2aPvideo Pvideo;
        Nv2aPtimer Ptimer;
        Nv2aPcounter Pcounter;
        Nv2aPvpe Pvpe;
        Nv2aPtv Ptv;
        Nv2aPrmfb Prmfb;
        Nv2aPrmvio Prmvio;
        Nv2aPfb Pfb;
        Nv2aPstraps Pstraps;
        Nv2aPgraph Pgraph;
        Nv2aPcrtc Pcrtc;
        Nv2aPrmcio Prmcio;
        Nv2aPramdac Pramdac;
        Nv2aPrmdio Prmdio;
        Nv2aPramin Pramin;
        Nv2aUser User;

        void UpdateIrq();
    private:
        std::vector<Nv2aRegion> m_MemoryRegions;
        Nv2aRegion* GetMemoryRegion(uint32_t addr);
        Xbox& m_Xbox;
        uint8_t* m_pRam;
};

#endif
