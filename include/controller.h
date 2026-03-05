#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <stdint.h>

float controller_calculate_phase_shift(uint16_t adc_result, float ref);

#endif
