#include "clock.h"

void clock_init(void)
{
        // Configure system clock to 160 MHz.
        uint32_t device_setclock_cfg = (SYSCTL_OSCSRC_XTAL | SYSCTL_IMULT(32) | SYSCTL_FMULT_NONE |
                                        SYSCTL_SYSDIV(2) | SYSCTL_PLL_ENABLE);
        SysCtl_setClock(device_setclock_cfg);

        // Configure XCLKOUT so that it reads CPU system clock with a 1 to 8 scale.
        SysCtl_selectClockOutSource(SYSCTL_CLOCKOUT_SYSCLK);
        SysCtl_setXClk(SYSCTL_XCLKOUT_DIV_8);

        // Connect GPIO73 to mux channel 3 which is XCLKOUT for GPIO73.
        GPIO_setPinConfig(GPIO_73_XCLKOUT);
        GPIO_setPadConfig(73U, GPIO_PIN_TYPE_STD);

        // Set GPIO15 to show OUTPUTXBAR4 signal.
        GPIO_setPinConfig(GPIO_15_OUTPUTXBAR4);
        GPIO_setPadConfig(15U, GPIO_PIN_TYPE_STD);
        GPIO_setDirectionMode(15U, GPIO_DIR_MODE_OUT);

        // Set XBAR so that XCLKOUT can be seen on GPIO15.
        XBAR_setInputPin(XBAR_INPUT6, 73U);
        XBAR_setOutputMuxConfig(XBAR_OUTPUT4, XBAR_OUT_MUX11_INPUTXBAR6);
        XBAR_enableOutputMux(XBAR_OUTPUT4, XBAR_MUX11);
}
