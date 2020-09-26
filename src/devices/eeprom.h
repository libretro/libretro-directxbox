#pragma once

#include "smbus_device.h"

class Eeprom : public SMDevice
{
   public:
      void Init();
      void Reset();

      void QuickCommand(bool read);
      uint8_t ReceiveByte();
      uint8_t ReadByte(uint8_t command);
      uint16_t ReadWord(uint8_t command);
      int ReadBlock(uint8_t command, uint8_t *data);

      void SendByte(uint8_t data);
      void WriteByte(uint8_t command, uint8_t value);
      void WriteWord(uint8_t command, uint16_t value);
      void WriteBlock(uint8_t command, uint8_t* data, int length);

   private:
      uint8_t m_Data[256];
};
