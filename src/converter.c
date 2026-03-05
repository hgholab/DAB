#include <math.h>

#include "../include/converter.h"

#include "../include/clock.h"

#define INPUT_VOLTAGE_NOM       48
#define OUTPUT_VOLTAGE_NOM      43
#define OUTPUT_VOLTAGE_MIN      33
#define OUTPUT_VOLTAGE_MAX      51
#define OUTPUT_POWER_NOM        120
#define OUTPUT_POWER_MAX        150
#define TRANSFORMER_TURNS_RATIO 1

// Maximum phase shift between primary and secondary is 80 degrees and the value of PHASE_SHIFT_MAX
// is defined to be (80 / 360).
#define PHASE_SHIFT_MAX 0.222222f

static double leakage_inductance      = 0.0;
float maximum_operational_phase_shift = 0.0f;

void converter_init(void)
{
        leakage_inductance = (((double)TRANSFORMER_TURNS_RATIO) * INPUT_VOLTAGE_NOM *
                              OUTPUT_VOLTAGE_MIN * PHASE_SHIFT_MAX * (1 - 2 * PHASE_SHIFT_MAX)) /
                             (DAB_SWITCHING_FREQ * OUTPUT_POWER_MAX);

        maximum_operational_phase_shift =
                (float)((0.25) *
                        (1.0 - sqrt(1.0 - (8.0 * DAB_SWITCHING_FREQ * leakage_inductance *
                                           OUTPUT_POWER_MAX) /
                                                  (TRANSFORMER_TURNS_RATIO * INPUT_VOLTAGE_NOM *
                                                   OUTPUT_VOLTAGE_MAX))));
}
