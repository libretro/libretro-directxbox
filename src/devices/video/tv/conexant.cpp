#include "conexant.h"
#include "util.h"

void ConexantTVEncoder::Init()
{

}

void ConexantTVEncoder::Reset()
{
}

void ConexantTVEncoder::QuickCommand(bool read)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::QuickCommand not implemented\n");
}

uint8_t ConexantTVEncoder::ReceiveByte()
{
   Log(LogLevel::Warning, "ConexantTVEncoder::ReceiveByte not implemented\n");
   return 0;
}

uint8_t ConexantTVEncoder::ReadByte(uint8_t command)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::ReadByte not implemented\n");
   return 0;
}

uint16_t ConexantTVEncoder::ReadWord(uint8_t command)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::ReadWord not implemented\n");
   return 0;
}

int ConexantTVEncoder::ReadBlock(uint8_t command, uint8_t *data)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::ReadBlock not implemented\n");
   return 0;
}

void ConexantTVEncoder::SendByte(uint8_t data)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::SendByte not implemented\n");
}

void ConexantTVEncoder::WriteByte(uint8_t command, uint8_t value)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::WriteByte not implemented\n");
}

void ConexantTVEncoder::WriteWord(uint8_t command, uint16_t value)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::WriteWord not implemented\n");
}

void ConexantTVEncoder::WriteBlock(uint8_t command, uint8_t* data, int length)
{
   Log(LogLevel::Warning, "ConexantTVEncoder::WriteBlock not implemented\n");
}
