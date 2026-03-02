#include <stdint.h>

#include "device.h"
#include "driverlib.h"

#include "../include/controller.h"

#define ADC_VREF        3.0f
#define MAX_PHASE_SHIFT 0.222222f // Maximum phase shift between primary and secondary in radians.
#define MIN_PHASE_SHIFT -MAX_PHASE_SHIFT

float controller_calculate_phase_shift(uint16_t adc_result, float ref)
{
        static float prev_output, prev_error = 0.0f;
        float output;

        float scaled_output_voltage = (adc_result / 4096.0f) * ADC_VREF;
        float output_voltage        = scaled_output_voltage * (80.0f / 3.3f);

        float error = ref - output_voltage;

        float b0 = 0.049907f;
        float b1 = -0.049792f;

        output = prev_output + b0 * error + b1 * prev_error;

        prev_error  = error;
        prev_output = output;

        output = output > MAX_PHASE_SHIFT ? MAX_PHASE_SHIFT :
                 output < MIN_PHASE_SHIFT ? MIN_PHASE_SHIFT :
                                            output;

        return output * 360.0f;
}
