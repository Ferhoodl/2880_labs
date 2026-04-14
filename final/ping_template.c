/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping_template.h"
#include "Timer.h"
#include "lcd.h"

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile enum{LOW, HIGH, DONE} g_state = LOW; // State of ping echo pulse

unsigned int count = 0;
bool intflag = true;


void ping_init (void){

        SYSCTL_RCGCGPIO_R |= 0x02;
        SYSCTL_RCGCTIMER_R |= 0x08;

        while ((SYSCTL_PRGPIO_R & 0x02) != 0x02) {}

        GPIO_PORTB_DIR_R &= ~0x08;
        GPIO_PORTB_AFSEL_R |= 0x08;
        GPIO_PORTB_PCTL_R |= 0x00007000;
        GPIO_PORTB_PCTL_R &= 0xFFFF7FFF;
        GPIO_PORTB_DEN_R |= 0x08;

        TIMER3_CTL_R &= ~0x100;
        TIMER3_CFG_R = 0x4;
        TIMER3_TBMR_R = 0x07;
        TIMER3_CTL_R |= (0x0C00);
        TIMER3_TBPR_R = 0xFF;
        TIMER3_TBILR_R = 0xFFFF;
        TIMER3_ICR_R |= 0x400;
        TIMER3_IMR_R |= 0x400;
        NVIC_EN1_R = 0x00000010;
        NVIC_PRI9_R = 0x8;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x0100; // CHECK THIS
}

void ping_trigger (void){
    g_state = LOW;

    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x0100;
    TIMER3_IMR_R &= ~0x0400;

    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x08;
    GPIO_PORTB_DIR_R   |=  0x08;

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DATA_R &= ~0x08;        // start with PB3 low
    timer_waitMicros(1);           // wait to go high
    GPIO_PORTB_DATA_R |=  0x08;        // then PB3 high
    timer_waitMicros(5);               // wait to go low for longer pulse
    GPIO_PORTB_DATA_R &= ~0x08;        // then PB3 low

    GPIO_PORTB_DIR_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R = 0x0400;

    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x08;
    TIMER3_IMR_R |= 0x0400;
    TIMER3_CTL_R |= 0x0100;
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE
  // As needed, go back to review your interrupt handler code for the UART lab.
  // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
  // includes checking the source of the interrupt and clearing the interrupt status bit.
  // Checking the source: test the MIS bit in the MIS register (is the ISR executing
  // because the input capture event happened and interrupts were enabled for that event?
  // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
  // The rest of the code in the ISR depends on actions needed when the event happens.
    if(TIMER3_MIS_R & 0x400) {

            TIMER3_ICR_R |= 0x400;

            if(g_state == LOW) {
                g_start_time = TIMER3_TBR_R;
                g_state = HIGH;

            } else if (g_state == HIGH) {
                g_end_time = TIMER3_TBR_R;
                g_state = DONE;

            }

        }
}

float ping_getDistance (void){
    ping_trigger();
    while (g_state != DONE) {};

    uint32_t timeDif = 0;

    if (g_start_time >= g_end_time) {
        timeDif = g_start_time - g_end_time;
    } else {
        timeDif = (0x1000000) + g_start_time - g_end_time;
        count++;
    }

    float timeMs = (timeDif / 16000000.0) * 1000.0;
    float distCm = ((timeDif / 16000000.0 / 2.0) * 343.0 * 100.0) - 2.0;

    lcd_printf("Cyc:%lu\nMs:%.2f\nDist:%.2f\nOVF:%u", timeDif, timeMs, distCm, count);
    return distCm;

}
