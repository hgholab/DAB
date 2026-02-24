#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/epwm.h"
#include "clock.h"

#define EPWM_NUM 4 // Number of ePWM modules used to control DAB converter switches

// This value represents the deadtime between 2 switches of each leg. The actual deadtime value
// should be calculated by DEADBAND * (1 / TBCLK).
#define DEADBAND 2

void epwm_init(void)
{
        // The address array for ePWM1, ePWM2, ePWM4, and ePWM5 base addresses.
        uint32_t epwms_base_addresses[EPWM_NUM] = {EPWM1_BASE, EPWM2_BASE, EPWM4_BASE, EPWM5_BASE};

        // Calculate EPWMCLK. By default it is half of the SYSCLK. The variable sysclk_frequency is
        // assigned its value in clock_init() function, so that function should be called before
        // calling this function.
        uint32_t EPWMCLK = sysclk_frequency / 2UL;

        // Time-base period (T_pwm = 2 * TBPRD * TBCLK and f_pwm = 1 / T_PWM for up-down count mode)
        uint16_t TBPRD = (uint16_t)(EPWMCLK / (2UL * DAB_SWITCHING_FREQ));

        // Counter-compare value for all four ePWMs for duty cycle of 50%, hence the division by 2
        uint16_t CMP = (uint16_t)(TBPRD / 2U);

        // Phase shift in degrees (This will be controlled by the ADC ISR)
        uint16_t phase_shift = 43;

        // Phase shift for ePWM4 with respect to ePWM1
        uint16_t TBPHS = (uint16_t)((2 * TBPRD) * (phase_shift / 360.0f));

        // Compensate for the delay between EPWM modules in the sync chain.
        TBPHS -= 2;

        // Stop TBCLK while configuring ePWMs.
        SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

        // Enable the clock for ePWM1, ePWM2, ePWM4, and ePWM5.
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM5);

        // Add 5 cycle delay before accessing peripheral registers.
        asm(" RPT #5 || NOP");

        // General configuration for all four ePWM modules
        size_t i;
        for (i = 0; i < EPWM_NUM; i++)
        {
                // Set the ePWM modules clock dividers to 1 so that TBCLK = EPWMCLK = SYSCLK / 2.
                EPWM_setClockPrescaler(
                        epwms_base_addresses[i], EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

                // Enable the time-base period shadow register mode.
                EPWM_setPeriodLoadMode(epwms_base_addresses[i], EPWM_PERIOD_SHADOW_LOAD);

                // Make shadow to active load to occur when time base counter reaches 0.
                EPWM_selectPeriodLoadEvent(epwms_base_addresses[i],
                                           EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO);

                // Set up the counter compare shadow load mode.
                EPWM_setCounterCompareShadowLoadMode(epwms_base_addresses[i],
                                                     EPWM_COUNTER_COMPARE_A,
                                                     EPWM_COMP_LOAD_ON_CNTR_ZERO);

                // Set the time-base period value.
                EPWM_setTimeBasePeriod(epwms_base_addresses[i], TBPRD);

                // Set the counter-compare register value.
                EPWM_setCounterCompareValue(epwms_base_addresses[i], EPWM_COUNTER_COMPARE_A, CMP);

                // Set the "Action Qualifer" action.
                EPWM_setActionQualifierAction(epwms_base_addresses[i],
                                              EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_HIGH,
                                              EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
                EPWM_setActionQualifierAction(epwms_base_addresses[i],
                                              EPWM_AQ_OUTPUT_A,
                                              EPWM_AQ_OUTPUT_LOW,
                                              EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

                // Deadtime inseration for each leg
                EPWM_setDeadBandDelayPolarity(
                        epwms_base_addresses[i], EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
                EPWM_setDeadBandDelayMode(epwms_base_addresses[i], EPWM_DB_RED, true);
                EPWM_setRisingEdgeDelayCount(epwms_base_addresses[i], DEADBAND);
                EPWM_setDeadBandDelayMode(epwms_base_addresses[i], EPWM_DB_FED, true);
                EPWM_setFallingEdgeDelayCount(epwms_base_addresses[i], DEADBAND);

                // Set the counting mode for all four ePWMs to up-down mode.
                EPWM_setTimeBaseCounterMode(epwms_base_addresses[i], EPWM_COUNTER_MODE_UP_DOWN);

                // Start all four time-base counters from zero.
                EPWM_setTimeBaseCounter(epwms_base_addresses[i], 0U);
        }

        // Make ePWM1 and ePWM4 send SYNCOUT pulse when their counters reach zero.
        EPWM_setSyncOutPulseMode(EPWM1_BASE, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);
        EPWM_setSyncOutPulseMode(EPWM4_BASE, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);

        // Make ePWM4 to be the slave of ePWM1.
        SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);

        // Ignore the synchronization input for ePWM1 since its the master.
        EPWM_disablePhaseShiftLoad(EPWM1_BASE);

        // Enable the synchronization input for ePWM2, ePWM4, and ePWM5 since they are slaves.
        EPWM_enablePhaseShiftLoad(EPWM2_BASE);
        EPWM_enablePhaseShiftLoad(EPWM4_BASE);
        EPWM_enablePhaseShiftLoad(EPWM5_BASE);

        // Set counter of slaves to count down after their master SYNCOUT event so that they are
        // behind their master.
        EPWM_setCountModeAfterSync(EPWM2_BASE, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);
        EPWM_setCountModeAfterSync(EPWM4_BASE, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);
        EPWM_setCountModeAfterSync(EPWM5_BASE, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);

        // Phase shift for ePWM2 so that it is shifted 180 degrees from ePWM1.
        // Whole period is 2TBPRD so 180 degrees is TBPRD. The shift value is subtracted by 2 for
        // better edge alignments.
        EPWM_setPhaseShift(EPWM2_BASE, TBPRD - 2);

        // Phase shift for ePWM4 and ePWM5.
        EPWM_setPhaseShift(EPWM4_BASE, TBPHS);
        EPWM_setPhaseShift(EPWM5_BASE, TBPRD - 2);

        // Start TBCLK. Now the time-base counter for all ePWM modules are synchronized.
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}
