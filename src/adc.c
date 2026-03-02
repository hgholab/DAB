#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/adc.h"

#include "../include/controller.h"
#include "../include/epwm.h"
#include "../include/gpio.h"

static void adc_soc_init(void);

float ref = 50.0f;

// The ADC ISR (interrupt service routine) which is responsible for updating the phase
// shift. At the end of ADC conversion adc_isr is invoked.
__interrupt void adc_isr(void)
{
        // In order to measure the time that CPU takes to do the control loop calculations, we
        // toggle a GPIO pin at the top and the bottom of this ISR, so that we can measure the time
        // passed between two toggles on the oscilloscope. For this purpose, we use GPIO 25 which is
        // easily accessible on LaunchXL f28379d.

        GPIO_writePin(ADC_ISR_TOGGLE_GPIO_PIN, 1);

        uint16_t adc_result = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

        // float phase_shift = controller_calculate_phase_shift(adc_result, ref);
        // uint16_t TBPHS    = (uint16_t)((2 * TBPRD) * (phase_shift / 360.0f));

        // EPWM_setPhaseShift(EPWM4_BASE, TBPHS);

        // float phase_shift = controller_calculate_phase_shift(adc_result, ref);
        float phase_shift;
        uint16_t TBPHS;
        // EPWM_setPhaseShift(EPWM4_BASE, TBPHS);

        if (adc_result >= 2048U)
        {
                phase_shift = 90;

                GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0); // blue LED on
                GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
        }
        else
        {
                phase_shift = 30;

                GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1); // blue LED off
                GPIO_writePin(DEVICE_GPIO_PIN_LED2, 0);
        }

        TBPHS = (uint16_t)((2 * TBPRD) * (phase_shift / 360.0f));
        EPWM_setPhaseShift(EPWM4_BASE, TBPHS);

        // adc_counter++;

        // // if (adc_counter % 200 == 0)
        // // {
        // if (is_on)
        // {
        //         // GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
        //         GPIO_writePin(ADC_ISR_TOGGLE_GPIO_PIN, 1);
        // }
        // else
        // {
        //         // GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);
        //         GPIO_writePin(ADC_ISR_TOGGLE_GPIO_PIN, 0);
        // }

        // is_on = !is_on;
        // // }

        DEVICE_DELAY_US(1);

        // Control loop calculations here
        GPIO_writePin(ADC_ISR_TOGGLE_GPIO_PIN, 0);

        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
        Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

void adc_init(void)
{
        // Enable the clock for ADC A.
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCA);

        // Add 5 cycle delay before accessing peripheral registers.
        asm(" RPT #5 || NOP");

        // Set the clock prescaler for ADC so that it does not exceed its maximum value of 50 MHz.
        // Since SYSCLK is set to be 200 MHz, then ADCCLK prescaler should be set to 4.
        ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);

        // Set resolution and signal mode of the ADC. In this device, single-ended signal
        // conversions are supported only in 12-bit resolution mode.
        ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

        // Set the timing of the end-of-conversion pulse so that the end of the conversion (EOC)
        // pusle happens at the end of the voltage conversion, one cycle prior to the ADC result
        // latching into it’s result register
        ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);

        // Associate the corresponding ISR to ADCA interrupt.
        Interrupt_register(INT_ADCA1, adc_isr);

        Interrupt_enable(INT_ADCA1);

        // Power up the ADC and then delay for 1 ms.
        ADC_enableConverter(ADCA_BASE);
        DEVICE_DELAY_US(1000);

        adc_soc_init();
}

static void adc_soc_init(void)
{
        ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN14, 15);
        ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
        ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
}
