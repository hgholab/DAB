#include <stdbool.h>
#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/timer.h"

#include "../include/clock.h"

#define CPU_TIMER1_FREQ 400000UL

uint32_t timer1_counter = 0UL;
bool led1_is_on         = false;

// CPU TIMER1 ISR
__interrupt void cpu_timer1_isr(void)
{
        CPUTimer_clearOverflowFlag(CPUTIMER1_BASE);

        timer1_counter++;

        // if (timer1_counter % 200 == 0)
        // {
        if (led1_is_on)
                GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
        else
                GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);

        led1_is_on = !led1_is_on;
        // }
}

// This function configures the TIMER1 so that it can be used to measure the execution time of
// different parts of code.
void timer_init(void)
{
        // Enable the clock for CPU TIMER1
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TIMER1);

        CPUTimer_stopTimer(CPUTIMER1_BASE);
        CPUTimer_setPreScaler(CPUTIMER1_BASE, 0U); // The same frequency as SYSCLK
        CPUTimer_setPeriod(CPUTIMER1_BASE, (sysclk_frequency / CPU_TIMER1_FREQ) - 1);
        CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);

        // Associate the corresponding ISR to TIMER1 interrupt.
        Interrupt_register(INT_TIMER1, cpu_timer1_isr);

        Interrupt_enable(INT_TIMER1);

        CPUTimer_enableInterrupt(CPUTIMER1_BASE);
        CPUTimer_clearOverflowFlag(CPUTIMER1_BASE);

        CPUTimer_startTimer(CPUTIMER1_BASE);
}
