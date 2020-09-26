#include "smc.h" 
#include "util.h"

void SMC::Init()
{
    m_PICVersionStringIndex = 0;
    buffer[SMC_COMMAND_SCRATCH] = 0;
}

void SMC::Reset()
{
}

void SMC::QuickCommand(bool read)
{
    Log(LogLevel::Warning, "SMC::QuickCommand not implemented");
}

uint8_t SMC::ReceiveByte()
{
    Log(LogLevel::Warning, "SMC::ReceiveByte not implemented");
    return 0;
}

uint8_t SMC::ReadByte(uint8_t command)
{
    switch (command) {
        case SMC_COMMAND_VERSION: // 0x01 PIC version string
            switch (m_revision) {
                case SCMRevision::P01: buffer[1] = "P01"[m_PICVersionStringIndex]; break;
                case SCMRevision::P2L: buffer[1] = "P05"[m_PICVersionStringIndex]; break; 
                case SCMRevision::D01: buffer[1] = "DXB"[m_PICVersionStringIndex]; break;
                case SCMRevision::D05: buffer[1] = "D05"[m_PICVersionStringIndex]; break;
            }

            m_PICVersionStringIndex = (m_PICVersionStringIndex + 1) % 3;
            break;
        case SMC_COMMAND_AV_PACK:       return 0x06; // TODO: Don't hard-code this
        case SMC_COMMAND_CHALLENGE_1C:  return 0x52;
        case SMC_COMMAND_CHALLENGE_1D:  return 0x72;
        case SMC_COMMAND_CHALLENGE_1E:  return 0xEA;
        case SMC_COMMAND_CHALLENGE_1F:  return 0x46;
    default:
        Log(LogLevel::Warning, "Unimplemented SMC Read 0x%02X\n", command);
    }

    return buffer[command];
}

uint16_t SMC::ReadWord(uint8_t command)
{
    Log(LogLevel::Warning, "SMC::ReadWord not implemented");
    return 0;
}

int SMC::ReadBlock(uint8_t command, uint8_t *data)
{
    Log(LogLevel::Warning, "SMC::ReadBlock not implemented");
    return 0;
}

void SMC::SendByte(uint8_t data)
{
    Log(LogLevel::Warning, "SMC::SendByte not implemented");
}

void SMC::WriteByte(uint8_t command, uint8_t value)
{
    switch (command) {
    case SMC_COMMAND_VERSION:
        if (value == 0) {
            m_PICVersionStringIndex = 0;
        }
        return;
    case SMC_COMMAND_RESET:
        return;
    case SMC_COMMAND_LED_SEQUENCE:
        Log(LogLevel::Warning, "SMC LED = %0X\n", value); break;
    case SMC_COMMAND_SCRATCH:
        Log(LogLevel::Warning, "SMC Scratch = %0X\n", value); break;
    default:
        Log(LogLevel::Warning, "Unimplemented SMC Write 0x%02X = 0x%02X\n", command, value);
        break;
    }

    buffer[command] = value;
}

void SMC::WriteWord(uint8_t command, uint16_t value)
{
    Log(LogLevel::Warning, "SMC::WriteWord not implemented");
}

void SMC::WriteBlock(uint8_t command, uint8_t* data, int length)
{
    Log(LogLevel::Warning, "SMC::WriteBlock not implemented");
}
