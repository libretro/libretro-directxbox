#ifndef _PIT_H_
#define _PIT_H_

class Xbox;

#include <cstdint>
#include <chrono>
#include <thread>

#define PORT_PIT_DATA_0        0x40
#define PORT_PIT_DATA_1        0x41
#define PORT_PIT_DATA_2        0x42
#define PORT_PIT_COMMAND    0x43

class I8254
{
   public:
      I8254(Xbox& xbox);
      ~I8254();
      void Reset();

      uint32_t IORead(uint32_t addr);
      void IOWrite(uint32_t addr, uint32_t value);
   private:
      Xbox& m_Xbox;
      bool m_Active;
      std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> GetNextInterruptTime();
      std::thread m_TimerThread;
      static void TimerThread(I8254* pPIT);
};

#endif
