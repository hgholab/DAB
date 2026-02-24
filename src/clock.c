#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/gpio.h"
#include "clock.h"

uint32_t sysclk_frequency; // This is the value of SYSCLK (PLLSYSCLK).

void clock_init(void)
{
        // Configure system clock to 160 MHz.
        uint32_t device_setclock_cfg = (SYSCTL_OSCSRC_XTAL | SYSCTL_IMULT(32) | SYSCTL_FMULT_NONE |
                                        SYSCTL_SYSDIV(2) | SYSCTL_PLL_ENABLE);
        SysCtl_setClock(device_setclock_cfg);

        // Store the SYSCLK frequency in a global variable so that is accessible to other files.
        sysclk_frequency = SysCtl_getClock(DEVICE_OSCSRC_FREQ);

        // Configure XCLKOUT so that it reads CPU system clock with a 1 to 8 scale.
        SysCtl_selectClockOutSource(SYSCTL_CLOCKOUT_SYSCLK);
        SysCtl_setXClk(SYSCTL_XCLKOUT_DIV_8);

        // Configure X-BAR so that XCLKOUT can be seen on GPIO15.
        XBAR_setInputPin(XBAR_INPUT6, XCLKOUT_GPIO_PIN);
        XBAR_setOutputMuxConfig(XBAR_OUTPUT4, XBAR_OUT_MUX11_INPUTXBAR6);
        XBAR_enableOutputMux(XBAR_OUTPUT4, XBAR_MUX11);
}
