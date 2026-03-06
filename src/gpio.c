#include "device.h"
#include "driverlib.h"

#include "../include/gpio.h"

#define EPWM1A_GPIO_PIN  0U
#define EPWM1B_GPIO_PIN  1U
#define EPWM2A_GPIO_PIN  2U
#define EPWM2B_GPIO_PIN  3U
#define EPWM4A_GPIO_PIN  6U
#define EPWM4B_GPIO_PIN  7U
#define EPWM5A_GPIO_PIN  8U
#define EPWM5B_GPIO_PIN  9U
#define SCITXDA_GPIO_PIN 42U
#define SCIRXDA_GPIO_PIN 43U

void gpio_init(void)
{
        /*
         * In order to be able to observe the PLLSYSCLK, which is the same as CPU1 and CPU2 SYSCLK,
         * on one of the available pins on LAUNCHXL-F28379D, we have to set GPIO73 MUX to XCLKOUT.
         * This signal (XCLKOUT) should not exceed 50 MHz, so in clock.c file it is configured to a
         * 1 to 8 frequency scale. So for example for a PLLSYSCLK of 200 MHz, it shows a 200 / 8 =
         * 25 MHz square wave.
         *
         * Since GPIO73 is not one of the physically available pins on LAUNCHXL-F28379D, we have to
         * use the crossbar (X-BAR) feature of TMS320F28379D (the MCU on the board), by which we can
         * route the signal from GPIO73 to another GPIO pin which has a OUTPUTXBARx as one of its
         * MUX channels and also is physically available on LAUNCHXL-F28379D. For this, we have
         * chosen GPIO15, which has OUTPUTXBAR4 as one of its MUX channels.
         */
        // Connect GPIO73 to XCLKOUT (MUX channel 3).
        GPIO_setPinConfig(GPIO_73_XCLKOUT);
        GPIO_setPadConfig(XCLKOUT_GPIO_PIN, GPIO_PIN_TYPE_STD);

        // Set GPIO15 to show OUTPUTXBAR4 signal.
        GPIO_setPinConfig(GPIO_15_OUTPUTXBAR4);
        GPIO_setPadConfig(OUTPUTXBAR4_GPIO_PIN, GPIO_PIN_TYPE_STD);

        /*
         * For the gate signals of primary and secondary bridges, we use ePWM modules. Primary
         * bridge uses ePWM1 and ePWM2, and secondary bridge uses ePWM4 and ePWM5.
         *
         * If we show the primary half-bridges by A and B, secondary half-bridges by C and D, and
         * higher-side and lower-side switches by H and L, primary has AH, AL, BH, and BL, and
         * secondary has CH, CL, DH, and DL switches. The following list shows the gate signal for
         * each switch:
         * 1. AH: ePWM1A
         * 2. AL: ePWM1B
         * 1. BH: ePWM2A
         * 2. BL: ePWM2B
         * 1. CH: ePWM4A
         * 2. CL: ePWM4B
         * 1. DH: ePWM5A
         * 2. DL: ePWM5B
         *
         * So for each ePWMxA and ePWMxB, we have to configure their corresponding GPIO pin.
         */
        GPIO_setPinConfig(GPIO_0_EPWM1A);
        GPIO_setPadConfig(EPWM1A_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_1_EPWM1B);
        GPIO_setPadConfig(EPWM1B_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_2_EPWM2A);
        GPIO_setPadConfig(EPWM2A_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_3_EPWM2B);
        GPIO_setPadConfig(EPWM2B_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_6_EPWM4A);
        GPIO_setPadConfig(EPWM4A_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_7_EPWM4B);
        GPIO_setPadConfig(EPWM4B_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_8_EPWM5A);
        GPIO_setPadConfig(EPWM5A_GPIO_PIN, GPIO_PIN_TYPE_STD);

        GPIO_setPinConfig(GPIO_9_EPWM5B);
        GPIO_setPadConfig(EPWM5B_GPIO_PIN, GPIO_PIN_TYPE_STD);

        // Configure GPIO pins for SCIA (UART) communication.
        // GPIO_setPinConfig(GPIO_42_SCITXDA);
        // GPIO_setPadConfig(SCITXDA_GPIO_PIN, GPIO_PIN_TYPE_STD);

        // GPIO_setPinConfig(GPIO_43_SCIRXDA);
        // GPIO_setQualificationMode(SCIRXDA_GPIO_PIN, GPIO_QUAL_ASYNC);

        //
        // GPIO43 is the SCI Rx pin.
        //
        GPIO_setMasterCore(43, GPIO_CORE_CPU1);
        GPIO_setPinConfig(GPIO_43_SCIRXDA);
        GPIO_setDirectionMode(43, GPIO_DIR_MODE_IN);
        GPIO_setPadConfig(43, GPIO_PIN_TYPE_PULLUP);
        GPIO_setQualificationMode(43, GPIO_QUAL_ASYNC);

        //
        // GPIO42 is the SCI Tx pin.
        //
        GPIO_setMasterCore(42, GPIO_CORE_CPU1);
        GPIO_setPinConfig(GPIO_42_SCITXDA);
        GPIO_setDirectionMode(42, GPIO_DIR_MODE_OUT);
        GPIO_setPadConfig(42, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(42, GPIO_QUAL_ASYNC);

        //
        // GPIO28 is the SCI Rx pin.
        //
        // GPIO_setMasterCore(28, GPIO_CORE_CPU1);
        // GPIO_setPinConfig(GPIO_28_SCIRXDA);
        // GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
        // GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
        // GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

        // //
        // // GPIO29 is the SCI Tx pin.
        // //
        // GPIO_setMasterCore(29, GPIO_CORE_CPU1);
        // GPIO_setPinConfig(GPIO_29_SCITXDA);
        // GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
        // GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
        // GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

        // GPIO configuration for LED1
        GPIO_setPinConfig(GPIO_31_GPIO31);
        GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
        GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);

        // GPIO configuration for LED2
        GPIO_setPinConfig(GPIO_34_GPIO34);
        GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
        GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);

        // Connect GPIO73 to XCLKOUT (MUX channel 3).
        GPIO_setPinConfig(GPIO_73_XCLKOUT);
        GPIO_setPadConfig(XCLKOUT_GPIO_PIN, GPIO_PIN_TYPE_STD);

        // Set GPIO14 to show OUTPUTXBAR3 signal.
        GPIO_setPinConfig(GPIO_14_OUTPUTXBAR3);
        GPIO_setPadConfig(OUTPUTXBAR3_GPIO_PIN, GPIO_PIN_TYPE_STD);

        // Configure ADCIN14 pin.
        // GPIO_setAnalogMode(uint32_t pin, GPIO_AnalogMode mode)

        GPIO_setPinConfig(GPIO_25_GPIO25);
        GPIO_setDirectionMode(25, GPIO_DIR_MODE_OUT);

        GPIO_setPinConfig(GPIO_27_GPIO27);
        GPIO_setDirectionMode(27, GPIO_DIR_MODE_OUT);

        GPIO_setPinConfig(GPIO_24_GPIO24);
        GPIO_setDirectionMode(24, GPIO_DIR_MODE_OUT);
}
