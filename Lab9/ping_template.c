/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping_template.h"
#include "Timer.h"

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile enum{LOW, HIGH, DONE} g_state = LOW; // State of ping echo pulse

void ping_init (void){

  // YOUR CODE HERE

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

}

float ping_getDistance (void){

    // YOUR CODE HERE
    return 0.0; // TODO: CHANGE THIS

}
