#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/gpio.h"
#include "clock.h"

uint32_t sysclk_frequency; // This is the value of SYSCLK (PLLSYSCLK).

void clock_init(void)
{
        // Configure system clock to 200 MHz.
        uint32_t device_setclock_cfg = (SYSCTL_OSCSRC_XTAL | SYSCTL_IMULT(40) | SYSCTL_FMULT_NONE |
                                        SYSCTL_SYSDIV(2) | SYSCTL_PLL_ENABLE);
        SysCtl_setClock(device_setclock_cfg);

        // Store the SYSCLK frequency in a global variable so that is accessible to other files.
        sysclk_frequency = SysCtl_getClock(DEVICE_OSCSRC_FREQ);

        // Configure XCLKOUT so that it reads CPU system clock with a 1 to 8 scale.
        SysCtl_selectClockOutSource(SYSCTL_CLOCKOUT_SYSCLK);
        SysCtl_setXClk(SYSCTL_XCLKOUT_DIV_8);
}
