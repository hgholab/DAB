// #include "../include/adc.h"

// #include <stddef.h>
// #include <stdint.h>

// #include "device.h"
// #include "driverlib.h"

// #include "../include/epwm.h"
// #include "clock.h"

// #define EPWM_NUM 4 // Number of ePWM modules used

// // This value represents the deadtime between 2 switches of each leg. The actual deadtime value
// // should be calculated by DEADBAND * (1 / TBCLK).
// #define DEADBAND 8

// void epwm_init(void)
// {
//         // The address array for ePWM1, ePWM2, ePWM4, and ePWM5 base addresses.
//         uint32_t epwms_base_address[EPWM_NUM] = {EPWM1_BASE, EPWM2_BASE, EPWM4_BASE, EPWM5_BASE};

//         // Calculate EPWMCLK. By default it is half of the SYSCLK.
//         uint32_t EPWMCLK = sysclk_frequency / 2UL;

//         // Time-base period (T_pwm = 2 * TBPRD * TBCLK and f_pwm = 1 / T_PWM for up-down count
//         mode) uint16_t TBPRD = (uint16_t)(EPWMCLK / (2UL * DAB_SWITCHING_FREQ));

//         // Counter-compare value for all four ePWMs for duty cycle of 50%, hence the division
//         by 2. uint16_t CMP = (uint16_t)(TBPRD / 2U);

//         // Phase shift in degrees
//         uint16_t phase_shift = 180;

//         // Calculate the phase shift for slave ePWMs (2, 4 and 5).
//         uint16_t TBPHS = (uint16_t)((2 * TBPRD) * (phase_shift / 360.0f));

//         // Compensate for the delay between EPWM modules in the sync chain so that for example AH
//         // and BL signals align perfectly.
//         TBPHS -= 2;

//         // Stop TBCLK while configuring ePWMs.
//         SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

//         // Enable the clock for ePWM1, ePWM2, ePWM4, and ePWM5.
//         SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1);
//         SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
//         SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);
//         SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM5);

//         // Add 5 cycle delay before accessing peripheral registers.
//         asm(" RPT #5 || NOP");

//         // Set the ePWM modules clock dividers to 1 so that TBCLK = EPWMCLK = SYSCLK / 2.
//         for (size_t i = 0; i < EPWM_NUM; i++)
//         {
//                 EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
//         }

//         EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
//         EPWM_setClockPrescaler(EPWM2_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
//         EPWM_setClockPrescaler(EPWM4_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
//         EPWM_setClockPrescaler(EPWM5_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);

//         // Enable the time-base period shadow register mode.
//         EPWM_setPeriodLoadMode(EPWM1_BASE, EPWM_PERIOD_SHADOW_LOAD);
//         EPWM_setPeriodLoadMode(EPWM2_BASE, EPWM_PERIOD_SHADOW_LOAD);
//         EPWM_setPeriodLoadMode(EPWM4_BASE, EPWM_PERIOD_SHADOW_LOAD);
//         EPWM_setPeriodLoadMode(EPWM5_BASE, EPWM_PERIOD_SHADOW_LOAD);

//         // Make shadow to active load to occur when time base counter reaches 0.
//         EPWM_selectPeriodLoadEvent(EPWM1_BASE, EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO);
//         EPWM_selectPeriodLoadEvent(EPWM2_BASE, EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO);
//         EPWM_selectPeriodLoadEvent(EPWM4_BASE, EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO);
//         EPWM_selectPeriodLoadEvent(EPWM5_BASE, EPWM_SHADOW_LOAD_MODE_COUNTER_ZERO);

//         // Set up the counter compare shadow load mode.
//         EPWM_setCounterCompareShadowLoadMode(
//                 EPWM1_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
//         EPWM_setCounterCompareShadowLoadMode(
//                 EPWM2_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
//         EPWM_setCounterCompareShadowLoadMode(
//                 EPWM4_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
//         EPWM_setCounterCompareShadowLoadMode(
//                 EPWM5_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);

//         // Make ePWM1 (master) send SYNCOUT pulse when its counter reaches zero.
//         EPWM_setSyncOutPulseMode(EPWM1_BASE, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);

//         // Make ePWM2, ePWM4, and ePWM5 to be the slave of the ePWM1.
//         SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
//         EPWM_setSyncOutPulseMode(EPWM4_BASE, EPWM_SYNC_OUT_PULSE_ON_EPWMxSYNCIN);

//         // Ignore the synchronization input for ePWM1 since its the master.
//         EPWM_disablePhaseShiftLoad(EPWM1_BASE);

//         // Enable the synchronization input for ePWM2, ePWM4, and ePWM5 since they are slaves.
//         EPWM_enablePhaseShiftLoad(EPWM2_BASE);
//         EPWM_enablePhaseShiftLoad(EPWM4_BASE);
//         EPWM_enablePhaseShiftLoad(EPWM5_BASE);

//         // Set counter of slaves to count down after ePWM1 SYNCOUT event so that they are behind
//         // ePWM1.
//         EPWM_setCountModeAfterSync(EPWM2_BASE, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);
//         EPWM_setCountModeAfterSync(EPWM4_BASE, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);
//         EPWM_setCountModeAfterSync(EPWM5_BASE, EPWM_COUNT_MODE_DOWN_AFTER_SYNC);

//         // Phase shift for slave modules.
//         EPWM_setPhaseShift(EPWM2_BASE, TBPHS);
//         EPWM_setPhaseShift(EPWM4_BASE, TBPHS * 2);
//         EPWM_setPhaseShift(EPWM5_BASE, TBPHS * 3);

//         // Set the time-base period value.
//         EPWM_setTimeBasePeriod(EPWM1_BASE, TBPRD);
//         EPWM_setTimeBasePeriod(EPWM2_BASE, TBPRD);
//         EPWM_setTimeBasePeriod(EPWM4_BASE, TBPRD);
//         EPWM_setTimeBasePeriod(EPWM5_BASE, TBPRD);

//         // Set the counter-compare register value for all four ePWMs so they have 50% duty cycle.
//         EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, CMP);
//         EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A, CMP);
//         EPWM_setCounterCompareValue(EPWM4_BASE, EPWM_COUNTER_COMPARE_A, CMP);
//         EPWM_setCounterCompareValue(EPWM5_BASE, EPWM_COUNTER_COMPARE_A, CMP);

//         // Set the "Action Qualifer" action for all four ePWM modules.
//         EPWM_setActionQualifierAction(EPWM1_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_HIGH,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
//         EPWM_setActionQualifierAction(EPWM1_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_LOW,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

//         EPWM_setActionQualifierAction(EPWM2_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_HIGH,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
//         EPWM_setActionQualifierAction(EPWM2_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_LOW,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

//         EPWM_setActionQualifierAction(EPWM4_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_HIGH,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
//         EPWM_setActionQualifierAction(EPWM4_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_LOW,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

//         EPWM_setActionQualifierAction(EPWM5_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_HIGH,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
//         EPWM_setActionQualifierAction(EPWM5_BASE,
//                                       EPWM_AQ_OUTPUT_A,
//                                       EPWM_AQ_OUTPUT_LOW,
//                                       EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

//         // Deadtime inseration for each leg so each leg makes active high complementary signals.
//         EPWM_setDeadBandDelayPolarity(EPWM1_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
//         EPWM_setDeadBandDelayMode(EPWM1_BASE, EPWM_DB_RED, true);
//         EPWM_setRisingEdgeDelayCount(EPWM1_BASE, DEADBAND);
//         EPWM_setDeadBandDelayMode(EPWM1_BASE, EPWM_DB_FED, true);
//         EPWM_setFallingEdgeDelayCount(EPWM1_BASE, DEADBAND);

//         EPWM_setDeadBandDelayPolarity(EPWM2_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
//         EPWM_setDeadBandDelayMode(EPWM2_BASE, EPWM_DB_RED, true);
//         EPWM_setRisingEdgeDelayCount(EPWM2_BASE, DEADBAND);
//         EPWM_setDeadBandDelayMode(EPWM2_BASE, EPWM_DB_FED, true);
//         EPWM_setFallingEdgeDelayCount(EPWM2_BASE, DEADBAND);

//         EPWM_setDeadBandDelayPolarity(EPWM4_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
//         EPWM_setDeadBandDelayMode(EPWM4_BASE, EPWM_DB_RED, true);
//         EPWM_setRisingEdgeDelayCount(EPWM4_BASE, DEADBAND);
//         EPWM_setDeadBandDelayMode(EPWM4_BASE, EPWM_DB_FED, true);
//         EPWM_setFallingEdgeDelayCount(EPWM4_BASE, DEADBAND);

//         EPWM_setDeadBandDelayPolarity(EPWM5_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);
//         EPWM_setDeadBandDelayMode(EPWM5_BASE, EPWM_DB_RED, true);
//         EPWM_setRisingEdgeDelayCount(EPWM5_BASE, DEADBAND);
//         EPWM_setDeadBandDelayMode(EPWM5_BASE, EPWM_DB_FED, true);
//         EPWM_setFallingEdgeDelayCount(EPWM5_BASE, DEADBAND);

//         // Set the counting mode for all four ePWMs to up-down mode.
//         EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
//         EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP_DOWN);
//         EPWM_setTimeBaseCounterMode(EPWM4_BASE, EPWM_COUNTER_MODE_UP_DOWN);
//         EPWM_setTimeBaseCounterMode(EPWM5_BASE, EPWM_COUNTER_MODE_UP_DOWN);

//         // Start all four time-base counters from zero.
//         EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);
//         EPWM_setTimeBaseCounter(EPWM2_BASE, 0U);
//         EPWM_setTimeBaseCounter(EPWM4_BASE, 0U);
//         EPWM_setTimeBaseCounter(EPWM5_BASE, 0U);

//         // Start TBCLK. Now the time-base counter for all ePWM modules are synchronized.
//         SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
// }
