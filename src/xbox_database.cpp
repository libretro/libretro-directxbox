#include "xbox_database.h"
#include <unordered_map>

std::vector<std::pair<std::string, MachineInfo>> Machines = {
   std::make_pair("v1_0_retail", MachineInfo{"v1.0 (Retail)", MachineType::Retail,  TvEncoderType::Conexant, McpxRevision::X3, McpxRomVersion::Mcpx1_0, }), 
   std::make_pair("v1_1_retail", MachineInfo{"v1.1 (Retail)", MachineType::Retail,  TvEncoderType::Conexant, McpxRevision::X3, McpxRomVersion::Mcpx1_1, }), 
   std::make_pair("v1_2_retail", MachineInfo{"v1.2 (Retail)", MachineType::Retail,  TvEncoderType::Conexant, McpxRevision::X3, McpxRomVersion::Mcpx1_1, }),
   std::make_pair("v1_3_retail", MachineInfo{"v1.3 (Retail)", MachineType::Retail,  TvEncoderType::Conexant, McpxRevision::X3, McpxRomVersion::Mcpx1_1, }),
   std::make_pair("v1_4_retail", MachineInfo{"v1.4 (Retail)", MachineType::Retail,  TvEncoderType::Focus,    McpxRevision::X3, McpxRomVersion::Mcpx1_1, }),
   std::make_pair("v1_6_retail", MachineInfo{"v1.6 (Retail)", MachineType::Retail,  TvEncoderType::XCalibur, McpxRevision::X3, McpxRomVersion::Mcpx1_1, }),
   std::make_pair("debug",       MachineInfo{"v1.0 (Debug)",  MachineType::Debug,   TvEncoderType::Conexant, McpxRevision::X2, McpxRomVersion::None,    }),
   std::make_pair("chihiro",     MachineInfo{"Chihiro",       MachineType::Chihiro, TvEncoderType::Conexant, McpxRevision::X2, McpxRomVersion::None,    }),
};
