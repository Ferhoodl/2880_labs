/*
 * adc.h
 *
 *  Created on: Mar 24, 2026
 *      Author: mmistry
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>

void adc_init(void);

uint32_t adc_read(void);


#endif /* ADC_H_ */
