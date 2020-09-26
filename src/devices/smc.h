#pragma once

#include "smbus_device.h"

#define SMC_COMMAND_VERSION 0x01
#define SMC_COMMAND_AV_PACK 0x04
#define SMC_COMMAND_CPU_TEMP 0x09
#define SMC_COMMAND_MOTHERBOARD_TEMP 0x0A 
#define SMC_COMMAND_POWER_FAN_READBACK 0x10
#define SMC_COMMAND_SCRATCH 0x1B
#define SMC_COMMAND_CHALLENGE_1C 0x1C
#define SMC_COMMAND_CHALLENGE_1D 0x1D
#define SMC_COMMAND_CHALLENGE_1E 0x1E
#define SMC_COMMAND_CHALLENGE_1F 0x1F

#define SMC_COMMAND_RESET 0x02
#define SMC_COMMAND_POWER_FAN_MODE 0x05
#define SMC_COMMAND_POWER_FAN_REGISTER 0x06
#define SMC_COMMAND_LED_MODE 0x07
#define SMC_COMMAND_LED_SEQUENCE 0x08
#define SMC_COMMAND_SCRATCH 0x1B

#define SMC_RESET_ASSERT_RESET            0x01
#define SMC_RESET_ASSERT_POWERCYCLE        0x40
#define SMC_RESET_ASSERT_SHUTDOWN        0x80

#define SMC_SCRATCH_TRAY_EJECT_PENDING    0x01
#define SMC_SCRATCH_DISPLAY_FATAL_ERROR    0x02
#define SMC_SCRATCH_SHORT_ANIMATION        0x04
#define SMC_SCRATCH_DASHBOARD_BOOT        0x08

typedef enum
{
    P01,
    P2L,
    D01, /* Seen in a debug kit  */
    D05, /* Seen in a earlier model chihiro */
} SCMRevision;

class SMC : public SMDevice
{
   public:
      /* SMDevice functions */
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
      SCMRevision m_revision = SCMRevision::P01;
      int m_PICVersionStringIndex = 0;
      uint8_t buffer[256] = {};
};
