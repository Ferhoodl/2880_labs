/*
 * adc.c
 *
 *  Created on: Mar 24, 2026
 *      Author: mmistry
 */

#include "adc.h"

#include "timer.h"



void adc_init(void){

    SYSCTL_RCGCADC_R |= 0x1;
        SYSCTL_RCGCGPIO_R |= 0x02;
        while ((SYSCTL_PRADC_R & 0x1) == 0) {}
        GPIO_PORTB_AFSEL_R |=  0x10;
        GPIO_PORTB_DEN_R   &= ~0x10;
        GPIO_PORTB_AMSEL_R |=  0x10;
        ADC0_ACTSS_R  &= ~0x4;          // disable SS2
        ADC0_EMUX_R   &= ~0x0F00;        // bits 11:8 = SS2 trigger, clear to 0 (processor)
        ADC0_SSMUX2_R  = 10;            // AIN10
        ADC0_SSCTL2_R  = 0x6;
        ADC0_ACTSS_R  |=  0x4;          //reenable SS2
}


uint32_t adc_read(void) {

       ADC0_PSSI_R  |= 0x4;                       // trigger SS2
       while ((ADC0_RIS_R & 0x4) == 0) {}         // check bit 2
       uint16_t result = ADC0_SSFIFO2_R & 0xFFF;  // read result
       ADC0_ISC_R   |= 0x4;                        // clear
       return result;
}


