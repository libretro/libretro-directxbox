#include "I8259.h"
#include "xbox.h"
#include "util.h"

#define ICW1 0
#define ICW2 1
#define ICW3 2
#define ICW4 3

#define ICW1_ICW4    0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE    0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4    0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL    0x08        /* Level triggered (edge) mode */
#define ICW1_INIT    0x10        /* Initialization - required! */

#define ICW4_8086    0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO    0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE    0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER    0x0C    /* Buffered mode/master */
#define ICW4_SFNM    0x10        /* Special fully nested (not) */

#define PIC_READ_IRR 0            /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 1            /* OCW3 irq service next CMD read */
#define PIC_EOI 0x20

// TODO: Implement ELCR support

I8259::I8259(Xbox& xbox) 
    : m_Xbox(xbox)
{

}

void I8259::Reset()
{
    Reset(PIC_MASTER);
    Reset(PIC_SLAVE);

    m_ELCR[PIC_MASTER] = 0;
    m_ELCR[PIC_SLAVE] = 0;

    m_ELCRMask[PIC_MASTER] = 0xF8;
    m_ELCRMask[PIC_SLAVE] = 0xDE;
}

void I8259::Reset(int pic)
{
    m_PreviousIRR[pic] = 0;
    m_IRR[pic] = 0;
    m_IMR[pic] = 0;
    m_ISR[pic] = 0;
    m_PriorityAdd[pic] = 0;
    m_Base[pic] = 0;
    m_ReadRegisterSelect[pic] = 0;
    m_Poll[pic] = false;
    m_SpecialMask[pic] = 0;
    m_InitState[pic] = 0;
    m_AutoEOI[pic] = false;
    m_RotateOnAutoEOI[pic] = false;
    m_IsSpecialFullyNestedMode[pic] = false;
    m_Is4ByteInit[pic] = false;
    m_InterruptOutput[pic] = false;
    UpdateIRQ(pic);
}

void I8259::RaiseIRQ(int index)
{
    if (index <= 7) {
        SetIRQ(PIC_MASTER, index, true);
    } else {
        SetIRQ(PIC_SLAVE, index - 8, true);
    }
}

void I8259::LowerIRQ(int index)
{
    if (index <= 7) {
        SetIRQ(PIC_MASTER, index, false);
    } else {
        SetIRQ(PIC_SLAVE, index - 8, false);
    }
}

void I8259::SetIRQ(int pic, int index, bool asserted)
{
    int mask = 1 << index;
    
    // Level Triggered
    if (m_ELCR[pic] & mask) {
        if (asserted) {
            m_IRR[pic] |= mask;
            m_PreviousIRR[pic] |= mask;
        } else {
            m_IRR[pic] &= ~mask;
            m_PreviousIRR[pic] &= ~mask;
        }

        UpdateIRQ(pic);
        return;
    }

    // Edge Triggered
    if (asserted) {
        if ((m_PreviousIRR[pic] & mask) == 0) {
            m_IRR[pic] |= mask;
        }
        m_PreviousIRR[pic] |= mask;
    } else {
        m_PreviousIRR[pic] &= ~mask;
    }

    UpdateIRQ(pic);
}

uint32_t I8259::IORead(uint32_t addr)
{
    switch (addr) {
        case PORT_PIC_MASTER_COMMAND:
            return CommandRead(PIC_MASTER);
        case PORT_PIC_SLAVE_COMMAND:
            return CommandRead(PIC_SLAVE);
        case PORT_PIC_MASTER_DATA:
            return DataRead(PIC_MASTER);
        case PORT_PIC_SLAVE_DATA:
            return DataRead(PIC_SLAVE);
        case PORT_PIC_MASTER_ELCR:
            return m_ELCR[PIC_MASTER];
        case PORT_PIC_SLAVE_ELCR:
            return m_ELCR[PIC_SLAVE];
    }

    return 0;
}

void I8259::IOWrite(uint32_t addr, uint32_t value)
{
    switch (addr) {
        case PORT_PIC_MASTER_COMMAND:
            CommandWrite(PIC_MASTER, value);
            break;
        case PORT_PIC_SLAVE_COMMAND:
            CommandWrite(PIC_SLAVE, value);
            break;
        case PORT_PIC_MASTER_DATA:
            DataWrite(PIC_MASTER, value);
            break;
        case PORT_PIC_SLAVE_DATA:
            DataWrite(PIC_SLAVE, value);
            break;
        case PORT_PIC_MASTER_ELCR:
            m_ELCR[PIC_MASTER] = value & m_ELCRMask[PIC_MASTER];
            break;
        case PORT_PIC_SLAVE_ELCR:
            m_ELCR[PIC_SLAVE] = value & m_ELCRMask[PIC_SLAVE];
            break;
    }
}

uint32_t I8259::CommandRead(int pic)
{
    if (m_Poll[pic]) {
        return Poll(pic);
    }

    if (m_ReadRegisterSelect) {
        return m_ISR[pic];
    }

    return m_IRR[pic];
}

void I8259::CommandWrite(int pic, uint32_t value)
{
    if (value & 0x10) {
        Reset(pic);
        m_InitState[pic] = 1;
        m_Is4ByteInit[pic] = value & 1;
        if (value & 0x08) {
            Log(LogLevel::Warning, "Level Sensitive IRQ Not Supported\n");
        }

        return;
    }

    if (value & 0x08) {
        if (value & 0x04) {
            m_Poll[pic] = true;
        }

        if (value & 0x02) {
            m_ReadRegisterSelect[pic] = value & 1;
        }
 
        if (value & 0x40) {
            m_SpecialMask[pic] = (value >> 5) & 1;
        }

        return;
    }

    int command = value >> 5;

    switch (command) {
        case 0:
        case 4:
            m_RotateOnAutoEOI[pic] = command >> 2;
            break;
        case 1:
        case 5: {
            int priority = GetPriority(pic, m_ISR[pic]);
            if (priority == 8) {
                return;
            }

            int irq = (priority + m_PriorityAdd[pic]) & 7;
            m_ISR[pic] &= ~(1 << irq);

            if (command == 5) {
                m_PriorityAdd[pic] = (irq + 1) & 7;
            }

            UpdateIRQ(pic);
            break;
        }
        case 3:{
            int irq = value & 7;
            m_ISR[pic] &= ~(1 << irq);
            UpdateIRQ(pic);
            break;
        }
        case 6:
            m_PriorityAdd[pic] = (value + 1) & 7;
            UpdateIRQ(pic);
            break;
        case 7:
            int irq = value & 7;
            m_ISR[pic] &= ~(1 << irq);
            m_PriorityAdd[pic] = (irq + 1) & 7;
            UpdateIRQ(pic);
            break;
    }
}

uint32_t I8259::DataRead(int pic)
{
    if (m_Poll[pic]) {
        return Poll(pic);
    }

    return m_IMR[pic];
}

void I8259::DataWrite(int pic, uint32_t value)
{
    switch (m_InitState[pic]) {
        case 0:
            m_IMR[pic] = value;
            UpdateIRQ(pic);
            break;
        case 1:
            m_Base[pic] = value & 0xF8;
            m_InitState[pic] = 2;
            break;
        case 2:
            if (m_Is4ByteInit[pic]) {
                m_InitState[pic] = 3;
            } else {
                m_InitState[pic] = 0;
            }
            break;
        case 3:
            m_IsSpecialFullyNestedMode[pic] = (value >> 4) & 1;
            m_AutoEOI[pic] = (value >> 1) & 1;
            m_InitState[pic] = 0;
            break;
    }
}

int I8259::GetCurrentIRQ()
{
    int masterIrq = GetIRQ(PIC_MASTER);

    // If this was a spurious IRQ, report it as such
    if (masterIrq < 0) {
        return m_Base[PIC_MASTER] + 7;
    }

    AcknowledgeIRQ(PIC_MASTER, masterIrq);

    // If the master IRQ didn't come from the slave
    if (masterIrq != 2) {
        return m_Base[PIC_MASTER] + masterIrq;
    }

    int slaveIrq = GetIRQ(PIC_SLAVE);

    // If slaveIrq was a spurious IRQ, report it as such
    if (slaveIrq < 0) {
        return m_Base[PIC_SLAVE] + 7;
    }

    AcknowledgeIRQ(PIC_SLAVE, slaveIrq);
    return m_Base[PIC_SLAVE] + slaveIrq;
}

int I8259::GetPriority(int pic, uint8_t mask)
{
    if (mask == 0) {
        return 8;
    }

    int priority = 0;
    while ((mask & (1 << ((priority + m_PriorityAdd[pic]) & 7))) == 0) {
        priority++;
    }

    return priority;
}

int I8259::GetIRQ(int pic)
{
    int mask = m_IRR[pic] & ~m_IMR[pic];
    int priority = GetPriority(pic, mask);
    if (priority == 8) {
        return -1;
    }

    mask = m_ISR[pic];

    if (m_SpecialMask[pic]) {
        mask &= ~m_IMR[pic];
    }

    if (m_IsSpecialFullyNestedMode[pic] && pic == PIC_MASTER) {
        mask &= ~(1 << 2);
    }

    int currentPriority = GetPriority(pic, mask);
    if (priority < currentPriority) {
        return (priority + m_PriorityAdd[pic]) & 7;
    } 
    
    return -1;
}

void I8259::AcknowledgeIRQ(int pic, int index)
{
    if (m_AutoEOI[pic]) {
        if (m_RotateOnAutoEOI[pic]) {
            m_PriorityAdd[pic] = (index + 1) & 7;
        }
    } else {
        m_ISR[pic] |= (1 << index);
    }

    if (!(m_ELCR[pic] & 1 << index)) {
        m_IRR[pic] &= ~(1 << index);
    }
    
    UpdateIRQ(pic);
}

uint8_t I8259::Poll(int pic)
{
    m_Poll[pic] = false;

    int irq = GetIRQ(pic);
    if (irq >= 0) {
        AcknowledgeIRQ(pic, irq);
        return irq | 0x80;
    }

    return 0;
}

void I8259::UpdateIRQ(int pic)
{
    int irq = GetIRQ(pic);

    if (irq >= 0) {
        m_InterruptOutput[pic] = true;
    } else {
        m_InterruptOutput[pic] = false;
    }

    // If this was the slave pic, cascade to master
    if (pic == PIC_SLAVE) {
        SetIRQ(PIC_MASTER, 2, m_InterruptOutput[pic]);
    }

    // If the master PIC has a pending interrupt, tell the Xbox
    if (pic == PIC_MASTER && m_InterruptOutput[PIC_MASTER]) {
        m_Xbox.Interrupt(GetCurrentIRQ());
    }
}