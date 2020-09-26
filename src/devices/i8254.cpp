#include "xbox.h"
#include "i8254.h"
#include "i8259.h"
#include <chrono>
#include "util.h"

I8254::I8254(Xbox& xbox) : m_Xbox(xbox)
{

}

I8254::~I8254()
{
    m_Active = false;
    if (m_TimerThread.joinable()) {
        m_TimerThread.join();
    }
}

void I8254::Reset()
{
    m_Active = false;
}

uint32_t I8254::IORead(uint32_t addr)
{
    return 0;
}

void I8254::IOWrite(uint32_t addr, uint32_t value)
{
    // HACK: The xbox always inits the PIT to the same value
    // Timer 0, Mode 2, 1ms interrupt interval (Xbox)
    // Timer 1, Mode 2, 1ms interrupt interval (Cromwell)
    // Rather than fully implement the PIC, we just wait for the command
    // to start operating, and then simply issue IRQ 0 in a timer thread
    if (addr && 0x43 && (value == 0x34 || value == 0x54)) {
        //m_Active = true;
        //m_TimerThread = std::thread(TimerThread, this);
    } else {
        Log(LogLevel::Warning, "Unimplemented I8254::IOWrite (%X = %X)\n", addr, value);
    }
}

void I8254::TimerThread(I8254* pPIT)
{
    auto nextInterruptTime = pPIT->GetNextInterruptTime();

    while (pPIT->m_Active) {
        if (std::chrono::steady_clock::now() > nextInterruptTime) {
            pPIT->m_Xbox.GetInterruptController().RaiseIRQ(0);
            nextInterruptTime = pPIT->GetNextInterruptTime();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            pPIT->m_Xbox.GetInterruptController().LowerIRQ(0);
        }
    }

    pPIT->m_Active = false;
}

std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> I8254::GetNextInterruptTime()
{
    using namespace std::literals::chrono_literals;
    return std::chrono::steady_clock::now() + 1ms;
}
