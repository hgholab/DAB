#include "epwm.h"

#define EPWM_PERIOD          (1.0f / DAB_SWITCHING_FREQ)
#define EPWMCLK              (SYSCLK / 2) // EPWMCLK = SYSCLK / 2 for SYSCLK > 100 MHz
#define EPWM_TBCLK           EPWMCLK
#define EPWM_TBCLK_PERIOD    (1.0f / EPWM_TBCLK)
#define EPWM_TBPRD           ((uint16_t)(PWM_PERIOD / (2 * EPWM_TBCLK_PERIOD)))
#define EPWM_DB_DELAY_COUNTS 20U

void configure_epwm_timebase(uint32_t base, bool is_master)
{
        // Stop (freeze) the timebase counter while configuring.
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_STOP_FREEZE);

        // Set timebase clock. EPWMCLK = SYSCLK / 2 for SYSCLK > 100 MHz, based on datasheet.
        EPWM_setClockPrescaler(base, EPWM_CLOCK_DIVIDER_2, EPWM_HSCLOCK_DIVIDER_1);

        // Set period for up-down count (center-aligned PWM).
        EPWM_setTimeBasePeriod(base, EPWM_TBPRD);

        // Start counter from zero.
        EPWM_setTimeBaseCounter(base, 0U);

        // Phase offset which is set to zero for now.
        EPWM_setPhaseShift(base, 0U);

        if (is_master)
        {
                // Master: disable sync input, generate sync output on CTR=0
                // EPWM_setSyncInPulseSource(base, EPWM_SYNC_IN_PULSE_SRC_DISABLE);
                // EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_COUNTER_COMPARE_C);

                EPWM_disablePhaseShiftLoad(base);

                EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_COUNTER_ZERO);

                // Master does not load counter from phase register.
                EPWM_disablePhaseShiftLoad(base);
        }
        else
        {
                // Slave: sync input comes from previous EPWM sync out
                // EPWM_setSyncInPulseSource(base, EPWM_SYNC_IN_PULSE_SRC_SYNCOUT_EPWM1);
                // EPWM_setSyncInPulseMode
                if (base == EPWM4_BASE)
                {
                        SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM4,
                                                  SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
                }

                // Load counter from phase register on sync pulse
                // EPWM_enablePhaseShiftLoad(base);

                // Count up after sync — important for correct phase relationship
                // EPWM_setCountModeAfterSync(base, EPWM_COUNT_MODE_UP_AFTER_SYNC);

                // Pass sync pulse downstream to next module in chain.
                EPWM_setSyncOutPulseMode(base, EPWM_SYNC_OUT_PULSE_ON_EPWMxSYNCIN);
        }

        // Up-down count for center-aligned PWM
        EPWM_setTimeBaseCounterMode(base, EPWM_COUNTER_MODE_UP_DOWN);
}

void configure_all_epwms(void)
{
        // Step 1 — Halt all EPWM timebases before configuration
        SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

        // Step 2 — Enable clocks to all 4 EPWM modules
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM1);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM3);
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);

        // Step 3 — Configure timebases
        configure_epwm_timebase(EPWM1_BASE, true);  // master
        configure_epwm_timebase(EPWM2_BASE, false); // slave
        configure_epwm_timebase(EPWM3_BASE, false); // slave
        configure_epwm_timebase(EPWM4_BASE, false); // slave

        // Step 4 — Release all timebases simultaneously
        // All 4 counters start together from this point
        SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void configureEPWMOutputs(uint32_t base)
{
        // ---------------------------------------------------
        // STEP 1: Set compare A to 50% duty (TBPRD/2)
        // This gives symmetric center-aligned PWM
        // Phase shift is applied via TBPHS, not CMPA
        // ---------------------------------------------------
        EPWM_setCounterCompareValue(base, EPWM_COUNTER_COMPARE_A, EPWM_TBPRD / 2U);

        // Shadow load on CTR = zero (center of up-down cycle)
        EPWM_setCounterCompareShadowLoadMode(
                base, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);

        // ---------------------------------------------------
        // STEP 2: Action Qualifier — EPWMxA
        // Up count, CTR = CMPA → Set HIGH
        // Down count, CTR = CMPA → Set LOW
        // ---------------------------------------------------
        EPWM_setActionQualifierAction(
                base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

        EPWM_setActionQualifierAction(
                base, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);

        // ---------------------------------------------------
        // STEP 3: Dead Band
        // Source both RED and FED from EPWMxA
        // EPWMxB = inverted and delayed EPWMxA (complementary)
        // ---------------------------------------------------

        // Input source for both delays is EPWMxA
        EPWM_setRisingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);
        EPWM_setFallingEdgeDeadBandDelayInput(base, EPWM_DB_INPUT_EPWMA);

        // Enable both RED and FED
        EPWM_setDeadBandDelayMode(base, EPWM_DB_RED, true);
        EPWM_setDeadBandDelayMode(base, EPWM_DB_FED, true);

        // Invert FED output so EPWMxB is complementary to EPWMxA
        // EPWMxA: active high (no inversion)
        // EPWMxB: active high after inversion
        EPWM_setDeadBandDelayPolarity(base, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_HIGH);
        EPWM_setDeadBandDelayPolarity(base, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);

        // Set delay counts (same for both edges for symmetry)
        EPWM_setRisingEdgeDelayCount(base, EPWM_DB_DELAY_COUNTS);
        EPWM_setFallingEdgeDelayCount(base, EPWM_DB_DELAY_COUNTS);

        // Dead band counter runs at full TBCLK rate
        EPWM_setDeadBandCounterClock(base, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
}

void configureAllEPWMOutputs(void)
{
        configureEPWMOutputs(EPWM1_BASE);
        configureEPWMOutputs(EPWM2_BASE);
        configureEPWMOutputs(EPWM3_BASE);
        configureEPWMOutputs(EPWM4_BASE);
}

void configureEPWMGPIOs(void)
{
        // EPWM1 - Primary bridge leg A
        GPIO_setPinConfig(GPIO_0_EPWM1A);
        GPIO_setPinConfig(GPIO_1_EPWM1B);

        // EPWM2 - Primary bridge leg B
        GPIO_setPinConfig(GPIO_2_EPWM2A);
        GPIO_setPinConfig(GPIO_3_EPWM2B);

        // EPWM3 - Secondary bridge leg A
        GPIO_setPinConfig(GPIO_4_EPWM3A);
        GPIO_setPinConfig(GPIO_5_EPWM3B);

        // EPWM4 - Secondary bridge leg B
        GPIO_setPinConfig(GPIO_6_EPWM4A);
        GPIO_setPinConfig(GPIO_7_EPWM4B);
}
