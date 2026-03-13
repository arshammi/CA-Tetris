#ifndef __ADC_H
#define __ADC_H

#include <string.h>
#include <stdint.h>


extern uint16_t AD_current; 

/* lib_adc.c */
void ADC_init (void);
void ADC_start_conversion (void);

/* IRQ_adc.c */
void ADC_IRQHandler(void);

#endif
