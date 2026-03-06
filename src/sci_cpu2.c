// #include "device.h"
// #include "driverlib.h"

// void main(void)
// {
//         uint16_t receivedChar;
//         unsigned char *msg;
//         uint16_t rxStatus  = 0U;
//         uint16_t ipcFlag17 = 17U;

//         //
//         // Configure PLL, disable WD, enable peripheral clocks.
//         //
//         // Device_init();

//         //
//         // Initialize interrupt controller and vector table.
//         //
//         // Interrupt_initModule();
//         // Interrupt_initVectorTable();

//         //
//         // Wait until CPU01 is ready and IPC flag 17 is set
//         //
//         while (!(HWREG(IPC_BASE + IPC_O_STS) & (1UL << ipcFlag17)))
//         {
//         }

//         //
//         // ACK IPC flag 17 for CPU1
//         //
//         HWREG(IPC_BASE + IPC_O_ACK) = 1UL << ipcFlag17;

//         //
//         // Initialize SCIA and its FIFO.
//         //
//         SCI_performSoftwareReset(SCIA_BASE);

//         //
//         // Configure SCIA for echoback.
//         //
//         SCI_setConfig(SCIA_BASE,
//                       DEVICE_LSPCLK_FREQ,
//                       9600,
//                       (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
//         SCI_resetChannels(SCIA_BASE);
//         SCI_resetRxFIFO(SCIA_BASE);
//         SCI_resetTxFIFO(SCIA_BASE);
//         SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
//         SCI_enableFIFO(SCIA_BASE);
//         SCI_enableModule(SCIA_BASE);
//         SCI_performSoftwareReset(SCIA_BASE);

// #ifdef AUTOBAUD
//         //
//         // Perform an autobaud lock.
//         // SCI expects an 'a' or 'A' to lock the baud rate.
//         //
//         SCI_lockAutobaud(SCIA_BASE);
// #endif

//         //
//         // Send starting message.
//         //
//         msg = "\r\n\n\nHello World!\0";
//         SCI_writeCharArray(SCIA_BASE, (uint16_t *)msg, 17);
//         msg = "\r\nYou will enter a character, and the DSP will echo it back!\n\0";
//         SCI_writeCharArray(SCIA_BASE, (uint16_t *)msg, 62);

//         for (;;)
//         {
//                 msg = "\r\nEnter a character: \0";
//                 SCI_writeCharArray(SCIA_BASE, (uint16_t *)msg, 22);

//                 //
//                 // Read a character from the FIFO.
//                 //
//                 receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

//                 rxStatus = SCI_getRxStatus(SCIA_BASE);
//                 if ((rxStatus & SCI_RXSTATUS_ERROR) != 0)
//                 {
//                         //
//                         // If Execution stops here there is some error
//                         // Analyze SCI_getRxStatus() API return value
//                         //
//                         ESTOP0;
//                 }

//                 //
//                 // Echo back the character.
//                 //
//                 msg = "  You sent: \0";
//                 SCI_writeCharArray(SCIA_BASE, (uint16_t *)msg, 13);
//                 SCI_writeCharBlockingFIFO(SCIA_BASE, receivedChar);

//                 //
//                 // Increment the loop count variable.
//                 //
//                 loopCounter++;
//         }
// }
