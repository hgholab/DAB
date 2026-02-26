#include "device.h"
#include "driverlib.h"

#include "../include/xbar.h"

#include "../include/gpio.h"

void xbar_init(void)
{
        // Configure X-BAR so that XCLKOUT can be seen on GPIO15.
        XBAR_setInputPin(XBAR_INPUT6, XCLKOUT_GPIO_PIN);
        XBAR_setOutputMuxConfig(XBAR_OUTPUT4, XBAR_OUT_MUX11_INPUTXBAR6);
        XBAR_enableOutputMux(XBAR_OUTPUT4, XBAR_MUX11);

        // Configure X-BAR so that XCLKOUT can be seen on GPIO15.
        XBAR_setInputPin(XBAR_INPUT5, LED1_GPIO_PIN);
        XBAR_setOutputMuxConfig(XBAR_OUTPUT3, XBAR_OUT_MUX09_INPUTXBAR5);
        XBAR_enableOutputMux(XBAR_OUTPUT3, XBAR_MUX09);
}
