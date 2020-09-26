#ifndef _BIOS_DATABASE_H
#define _BIOS_DATABASE_H

#include <string>
#include <vector>

enum class TvEncoderType {
    Conexant,
    Focus,
    XCalibur
};

enum class McpxRevision {
    X2,
    X3
};

enum class McpxRomVersion {
    None,
    Mcpx1_0,
    Mcpx1_1
};

enum class MachineType {
    Retail,
    Debug,
    Chihiro
};

typedef struct {
    std::string name;
    MachineType machine;
    TvEncoderType tv_encoder;
    McpxRevision mcpx;
    McpxRomVersion mcpx_rom;
} MachineInfo;

#endif