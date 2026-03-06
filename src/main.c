#include "device.h"
#include "driverlib.h"

#include "../include/adc.h"
#include "../include/clock.h"
#include "../include/converter.h"
#include "../include/epwm.h"
#include "../include/gpio.h"
#include "../include/sci.h"
#include "../include/timer.h"
#include "../include/xbar.h"

void system_init(void)
{
        // Device_init(); // From ti's device.c file
        clock_init();

        Device_initGPIO(); // From ti's device.c file
        gpio_init();

        xbar_init();

        Interrupt_initModule();
        Interrupt_initVectorTable();
        Interrupt_enableGlobal();

        // timer_init();

        epwm_init();

        adc_init();
}

int main(void)
{

        system_init();
        GPIO_writePin(LED1_GPIO_PIN, 1);
        GPIO_writePin(LED2_GPIO_PIN, 1);
        converter_init();
        sci_init();

        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

        for (;;)
        {
                // GPIO_writePin(27U, 1);
                // DEVICE_DELAY_US(10);
                // GPIO_writePin(27U, 0);
                // DEVICE_DELAY_US(10);

                //         GPIO_writePin(24U, 1);
                //         DEVICE_DELAY_US(10);
                //         GPIO_writePin(24U, 0);
                //         DEVICE_DELAY_US(10);
        }
}
