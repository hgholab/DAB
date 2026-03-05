#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

#define DAB_SWITCHING_FREQ 350000UL

extern uint32_t sysclk_frequency;

void clock_init(void);

#endif
