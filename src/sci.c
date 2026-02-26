#include "device.h"
#include "driverlib.h"

#include "../include/sci.h"

void sci_init(void)
{
        // Enable the clock for SCIA
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_SCIA);

        // Add 5 cycle delay before accessing peripheral registers.
        asm(" RPT #5 || NOP");

        //
        // Hold/disable module while configuring (optional but clean)
        //
        SCI_disableModule(SCIA_BASE);

        //
        // 8 data bits, 1 stop bit, no parity @ 115200 baud using LSPCLK = 50 MHz
        //
        SCI_setConfig(SCIA_BASE,
                      50000000UL,
                      115200UL,
                      (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));

        //
        // Idle-line protocol (matches your comment)
        //
        SCI_setIdleMultiProcessorMode(SCIA_BASE);

        //
        // Enable TX and RX interrupts (TXRDY, RXRDY/BRKDT)
        //
        SCI_enableInterrupt(SCIA_BASE, (SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT));

        //
        // Take SCI out of reset and enable TX/RX
        //
        SCI_enableModule(SCIA_BASE);
}
