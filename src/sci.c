#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/sci.h"

#include "../include/adc.h"
#include "../include/utils.h"

#define SCI_BUFFER_LENGTH 16

static uint16_t sci_buffer[SCI_BUFFER_LENGTH];
static int sci_buffer_index = 0;

static void sci_clear_buffer(void);
static void sci_clear_terminal(void);
static void sci_print_ref_message(void);

__interrupt void sci_rx_isr(void)
{
        uint16_t ch = SCI_readCharNonBlocking(SCIA_BASE) & 0x00FF;

        SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXRDY_BRKDT);
        Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

        if (ch == '\r' || ch == '\n')
        {
                SCI_writeCharBlockingNonFIFO(SCIA_BASE, '\n');
                SCI_writeCharBlockingNonFIFO(SCIA_BASE, '\r');

                float new_ref_value = str_to_float(sci_buffer);

                sci_clear_buffer();

                sci_print_ref_message();

                adc_set_ref(new_ref_value);
        }
        else if (ch == '\b')
        {
                if (sci_buffer_index != 0)
                {
                        sci_buffer[--sci_buffer_index] = 0U;
                        SCI_writeCharBlockingNonFIFO(SCIA_BASE, '\b');
                        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ' ');
                        SCI_writeCharBlockingNonFIFO(SCIA_BASE, '\b');
                }
        }
        else
        {
                if (ch == 46U || (ch >= 48U && ch <= 57U))
                {
                        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ch);
                        sci_buffer[sci_buffer_index++] = ch;
                }
        }
}

void sci_init(void)
{
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SCIA);
        asm(" RPT #5 || NOP");

        SCI_disableModule(SCIA_BASE);

        SCI_disableFIFO(SCIA_BASE);

        SCI_setConfig(SCIA_BASE,
                      SysCtl_getLowSpeedClock(DEVICE_OSCSRC_FREQ),
                      115200U,
                      (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));

        SCI_resetChannels(SCIA_BASE);

        Interrupt_register(INT_SCIA_RX, sci_rx_isr);

        SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT);
        SCI_disableInterrupt(SCIA_BASE, SCI_INT_TXRDY);
        SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);

        SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXRDY_BRKDT | SCI_INT_RXERR);
        SCI_clearOverflowStatus(SCIA_BASE);

        Interrupt_enable(INT_SCIA_RX);

        SCI_enableModule(SCIA_BASE);

        sci_clear_terminal();
        sci_print_ref_message();
}

static void sci_clear_buffer(void)
{
        sci_buffer_index = 0;

        for (int i = 0; i < SCI_BUFFER_LENGTH; i++)
        {
                sci_buffer[i] = 0U;
        }
}

static void sci_clear_terminal(void)
{
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 27);
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, '[');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, '2');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'J');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 27);
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, '[');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'H');
}

static void sci_print_ref_message(void)
{
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'E');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'n');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 't');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'r');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ' ');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 't');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'h');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ' ');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'n');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'w');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ' ');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'r');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'f');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'r');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'n');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'c');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, 'e');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ':');
        SCI_writeCharBlockingNonFIFO(SCIA_BASE, ' ');
}
