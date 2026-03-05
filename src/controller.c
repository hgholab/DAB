#include "device.h"
#include "driverlib.h"

#include "../include/controller.h"

#include "../include/converter.h"

#define ADC_VREF 3.0f

//
float controller_calculate_phase_shift(uint16_t adc_result, float ref)
{
        static float prev_output, prev_error = 0.0f;
        float output;

        // When the output is close to zero limit the phase shift so that the controller does not
        // push the phase shift to the maximum value possible at the start. Also modify the
        // controller static varaibles (prev_error and prev_output) to have a smoother transition
        // when adc_result gets bigger.
        if (adc_result < 200U)
        {
                prev_error  = 0.0f;
                prev_output = maximum_operational_phase_shift / 10;
                return maximum_operational_phase_shift / 10;
        }

        float scaled_output_voltage = (adc_result / 4096.0f) * ADC_VREF;
        float output_voltage        = scaled_output_voltage * (80.0f / 3.3f);

        float error = ref - output_voltage;

        // Compensator z-transform numerator coefficents
        float b0 = 0.049878f;
        float b1 = -0.049821f;

        // z-transform of the compensator:
        //
        //      Y(z)          b0 + b1 z^-1
        //      ----  =  ----------------------
        //      E(z)           1 - z^-1
        //
        // so y[k] = y[k - 1] + b0 * e[k] + b1 * e[k - 1]
        output = prev_output + b0 * error + b1 * prev_error;

        // Update the state for the next iteration of the controller.
        prev_error  = error;
        prev_output = output;

        // Limit the phase shift. No power transfer from secondary to primary, so we do not allow
        // the phase shift to go negative.
        output = output >= maximum_operational_phase_shift ? maximum_operational_phase_shift :
                 output <= 0.0f                            ? 0.0f :
                                                             output;

        // Return the phase shift in degrees.
        return output * 360.0f;
}
