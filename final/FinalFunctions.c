/*
 * FinalFunctions.c
 *
 *  Created on: Apr 14, 2026
 *      Author: mmistry
 */

#include "FinalFunctions.h"
#include "Timer.h"
#include "lcd.h"
#include "stdio.h"


// ----------------------------------------------------------------
// ADC
//-----------------------------------------------------------------

void adc_init(void){

        SYSCTL_RCGCADC_R |= 0x1;
        SYSCTL_RCGCGPIO_R |= 0x02;
        while ((SYSCTL_PRADC_R & 0x1) == 0) {}
        GPIO_PORTB_AFSEL_R |=  0x10;
        GPIO_PORTB_DEN_R   &= ~0x10;
        GPIO_PORTB_AMSEL_R |=  0x10;

        ADC0_ACTSS_R  &= ~0x4;          // disable SS2
        ADC0_SAC_R = 0x4; // sample size of 16
      // ADC0_RIS_R
      // ADC0_IM_R
      // ADC0_ISC_R
        ADC0_EMUX_R   &= ~0x0F00;        // bits 11:8 = SS2 trigger, clear to 0 (processor)
      // ADC0_PSSI_R
        ADC0_SSMUX2_R  = 10;            // AIN10
        ADC0_SSCTL2_R  = 0x6;
       // ADC0_SSFIFO2_R
       // ADC0_CC_R
        ADC0_ACTSS_R  |=  0x4;          //reenable SS2
}

uint32_t adc_read(void) {

       ADC0_PSSI_R  |= 0x4;                       // trigger SS2
       while ((ADC0_RIS_R & 0x4) == 0) {}         // check bit 2
       uint16_t result = ADC0_SSFIFO2_R & 0xFFF;  // read result
       ADC0_ISC_R   |= 0x4;                        // clear
       return result;
}


// ------------------------------------------------------------------
// Button
// ------------------------------------------------------------------

void button_init() {
	static uint8_t initialized = 0;
	timer_init();

	//Check if already initialized
	if(initialized){
		return;
	}

	SYSCTL_RCGCGPIO_R |= 0b010000;     // turn on the system clock to port e.
	timer_waitMicros(1);               // delay for previous instruction to finish?
	GPIO_PORTE_DIR_R &= 0xFFFFFFF0;    // set buttons 0, 1, 2, and 3 as input
	GPIO_PORTE_DEN_R |= 0b1111;        // enable lines 0, 1, 2, and 3


	initialized = 1;
}

uint8_t button_getButton() {

	// TODO: Write code below -- Return the rightmost button position pressed

	// INSERT CODE HERE!

    int button0Val;
    int button1Val;
    int button2Val;
    int button3Val;

    int masked0 = GPIO_PORTE_DATA_R | 0b1110;
    int masked1 = GPIO_PORTE_DATA_R | 0b1101;
    int masked2 = GPIO_PORTE_DATA_R | 0b1011;
    int masked3 = GPIO_PORTE_DATA_R | 0b0111;

    if(masked0 == 0b1110){button0Val = 1; }else{button0Val = 0; }
    if(masked1 == 0b1101){button1Val = 1; }else{button1Val = 0; }
    if(masked2 == 0b1011){button2Val = 1; }else{button2Val = 0; }
    if(masked3 == 0b0111){button3Val = 1; }else{button3Val = 0; }

    if(button3Val){
        return 4;
    }else if(button2Val){
        return 3;
    }else if(button1Val){
        return 2;
    }else if(button0Val){ // 0 is the port, 1 is the label on the button
        return 1;
    }

	return 0; // EDIT ME
}


// -------------------------------------------------------------------
// Movement
// -------------------------------------------------------------------

void benchAngle(oi_t *sensor_data, movementTunes *t, int rotations){
    turn_right(sensor_data, t, rotations * 360);
}

void escapeLeft(oi_t *sensor_data, movementTunes *t){
    move_backward(sensor_data, t, 150);
    turn_left(sensor_data, t, 90);
    move_forward(sensor_data, t, 250);
    turn_right(sensor_data, t, 90);
    move_forward(sensor_data, t, 150);
}

void escapeRight(oi_t *sensor_data, movementTunes *t){
    move_backward(sensor_data, t, 150);
    turn_right(sensor_data, t, 90);
    move_forward(sensor_data, t, 250);
    turn_left(sensor_data, t, 90);
    move_forward(sensor_data, t, 150);
}

void move_forward(oi_t *sensor_data, movementTunes *t, double distance_mm){
    double sum = 0;
    double wheelspeed = 50; // mm/sec
    double driveAdjustVal = ((t->driveDriftMultiplier * wheelspeed) / 2);
    oi_setWheels(wheelspeed + driveAdjustVal, wheelspeed - driveAdjustVal); // applying driveAdjustVal, the right is driveDriftMultiplier% more than the left
        while (sum < distance_mm){
            oi_update(sensor_data);
            sum += sensor_data -> distance * t->driveDistanceMultiplier;

            lcd_printf("dist: %f", sum * t->driveDistanceMultiplier + 1);
        }
    oi_setWheels(0,0);
}

void move_backward(oi_t *sensor_data, movementTunes *t, double distance_mm){
    double sum = 0;
    oi_setWheels(-50,-50);
        while (sum < distance_mm){
            oi_update(sensor_data);
            sum += abs(sensor_data -> distance * t->driveDistanceMultiplier);

            lcd_printf("dist: %f", -sum);
        }
    oi_setWheels(0,0);
}

void turn_right(oi_t *sensor_data, movementTunes *t, double degrees){
    oi_update(sensor_data);
    double angleDiff = 0.0;
    while(abs(angleDiff) < degrees * t->turnAngleMultiplier){
        oi_setWheels(-50,50);
        angleDiff += sensor_data->angle;
        lcd_printf("angle: %f", angleDiff);
        oi_update(sensor_data);
    }
    oi_setWheels(0,0);

}

void turn_left(oi_t *sensor_data, movementTunes *t, double degrees){

    oi_update(sensor_data);
    double angleDiff = 0;
    while(abs(angleDiff) < degrees * t->turnAngleMultiplier){
        oi_setWheels(50,-50);
        angleDiff += sensor_data->angle;
        lcd_printf("angle: %f", angleDiff);
        oi_update(sensor_data);
    }
    oi_setWheels(0,0);
}


//---------------------------------------------------------------------
// Ping Sensor
// --------------------------------------------------------------------

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


// -----------------------------------------------------------------------------
// Servo
// -----------------------------------------------------------------------------

void servo_init(void){

    SYSCTL_RCGCTIMER_R |= 0x02;   // Timer 1
       SYSCTL_RCGCGPIO_R  |= 0x02;   // Port B
       while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}  // wait for Port B ready
       GPIO_PORTB_DIR_R    |= 0x20;   // PB5 output
       GPIO_PORTB_DEN_R    |= 0x20;   // digital enable
       GPIO_PORTB_AFSEL_R  |= 0x20;   // alternate function
       GPIO_PORTB_PCTL_R   = (GPIO_PORTB_PCTL_R & ~0x00F00000) | 0x00700000;

       TIMER1_CTL_R &= ~0x0100;       // Disable Timer 1B before configuration
       TIMER1_CFG_R = 0x04;    // Set 16-bit mode (GPTMCFG = 4 split mode)

       TIMER1_TBMR_R = (TIMER1_TBMR_R & ~0xFF) | 0x0A;
       TIMER1_TBPMR_R |= 0xB;
       TIMER1_CTL_R &= ~0x4000;
       TIMER1_TBILR_R = 0xE200;
       TIMER1_TBPR_R = 0x4;
       TIMER1_TBMATCHR_R = 0x8440; // set this to 90 degrees
       TIMER1_TBPMR_R = 0x4; // set this to 90 degrees (you need the formulas and set total clock cycles)
       TIMER1_CTL_R |= 0x0100;
}

void servo_move(uint16_t degrees){
    double highMillis = (double)degrees/180.0 + 1;              // number of milliseconds we want the high to be to achieve the given degrees
    uint32_t highClockCycles = ((double)highMillis/1000.0)/(6.25 * 0.00000001); // number of clock cycles for above millis
    uint32_t lowClockCycles =  0x4E200 - highClockCycles;// 20 (0x4E200) ms  - highClockMillis (we set the register with lowClockCycles. That causes the high to be highClockCycles).

    uint16_t first16 = 0xFFFF & lowClockCycles;
    uint16_t last4 = lowClockCycles >> 16;

    TIMER1_TBMATCHR_R = first16; // gets the first 16 bits of the value (we have to split between these two registers)
    TIMER1_TBPMR_R = last4; // gets the last 4 bits of the value

    lcd_printf("Hdeg: %d\nHms: %f\nHcycs: %d", degrees, highMillis, highClockCycles);

    // this code is currently finding for the number of cycles for the high. We need to set the value for the low. Probably just do initial - clockCycles;



}


// -----------------------------------------------------------------------------
// UART (basic)
// -----------------------------------------------------------------------------

void uart_init(void){
	//TODO
  //enable clock to GPIO port B
  SYSCTL_RCGCGPIO_R |= 0x02;

  //enable clock to UART1
  SYSCTL_RCGCUART_R |= 0x02; // Bai textbook page 662

  //wait for GPIOB and UART1 peripherals to be ready
  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
  while ((SYSCTL_PRUART_R & 0x02) == 0) {};

  //enable alternate functions on port B pins
  GPIO_PORTB_AFSEL_R |= 0x03;

  //enable digital functionality on port B pins
  GPIO_PORTB_DEN_R |= 0x03;

  //enable UART1 Rx and Tx on port B pins
  //GPIO_PORTB_PCTL_R = ??? <-- This line was is from this file, but the line below is from the lab5_template that we originally wrote
  //GPIO_PORTB_PCTL_R |= 0x00000011;
  GPIO_PORTB_PCTL_R &= 0xFFFFFF00;       // Force 0's in the desired locations
  GPIO_PORTB_PCTL_R |= 0x00000011;       // Force 1's in the desired locations

  //calculate baud rate
  uint16_t iBRD = 8; //use equations (got values from Bai textbook page 662)
  uint16_t fBRD = 44; //use equations (got values from Bai textbook page 662)

  //turn off UART1 while setting it up
  UART1_CTL_R &= 0xFFFFFFFE; // got from Bai textbook page 662

  //set baud rate
  //note: to take effect, there must be a write to LCRH after these assignments
  UART1_IBRD_R = iBRD;
  UART1_FBRD_R = fBRD;

  //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
  //note: this write to LCRH must be after the BRD assignments
  UART1_LCRH_R = 0x60; // got from Bai textbook page 662

  //use system clock as source
  //note from the datasheet UARTCCC register description:
  //field is 0 (system clock) by default on reset
  //Good to be explicit in your code
  UART1_CC_R = 0x00;

  //re-enable UART1 and also enable RX, TX (three bits)
  //note from the datasheet UARTCTL register description:
  //RX and TX are enabled by default on reset
  //Good to be explicit in your code
  //Be careful to not clear RX and TX enable bits
  //(either preserve if already set or set them)
  UART1_CTL_R |= 0b1100000001; // UART1_CTL_R = ??? was the original line, but page 662 of Bai textbook uses what is not commented

}
/*
void uart_sendChar(char data){
	while((UART1_FR_R & 0x20) != 0);
    UART1_DR_R = data;

	}
*/
  char uart_receive(void){
    while ((UART1_FR_R & 0x10) != 0);
    return (char) (UART1_DR_R & 0xFF);

}
/*
void uart_sendStr(const char *data) {
    while (*data != '\0') {
        uart_sendChar(*data);
        data++;
    }
}
*/
char uart_receive_nonblocking(void){
    int test = (UART_FR_RXFE == 1);
    if(test == 1){              // UART_FR_RXFE as 1 means "UART Receive FIFO Empty"
        return '0';
    }else{
        return (char) (UART1_DR_R & 0xFF);
    }
}


// -----------------------------------------------------------------------------
// UART Interrupt
// -----------------------------------------------------------------------------


extern volatile char currentChar;

// These variables are declared as examples for your use in the interrupt handler.
volatile char command_byte = -1; // byte value for special character used as a command
volatile int command_flag = 0; // flag to tell the main program a special command was received

void uart_interrupt_init(void){
	//TODO
  //enable clock to GPIO port B
  SYSCTL_RCGCGPIO_R |= 0x02;

  //enable clock to UART1
  SYSCTL_RCGCUART_R |= 0x02;

  //wait for GPIOB and UART1 peripherals to be ready
  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
  while ((SYSCTL_PRUART_R & 0x02) == 0) {};

  //enable digital functionality on port B pins
  GPIO_PORTB_DEN_R |= 0x03;

  //enable alternate functions on port B pins
  GPIO_PORTB_AFSEL_R |= 0x03;

  //enable UART1 Rx and Tx on port B pins
  GPIO_PORTB_PCTL_R &= 0xFFFFFF00;       // Force 0's in the desired locations
  GPIO_PORTB_PCTL_R |= 0x00000011;       // Force 1's in the desired locations

  //calculate baud rate
  uint16_t iBRD = 8; //use equations (got values from Bai textbook page 662)
  uint16_t fBRD = 44; //use equations (got values from Bai textbook page 662)

  //turn off UART1 while setting it up
  UART1_CTL_R &= 0xFFFFFFFE; // got from Bai textbook page 662

  //set baud rate
  //note: to take effect, there must be a write to LCRH after these assignments
  UART1_IBRD_R = iBRD;
  UART1_FBRD_R = fBRD;

  //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
  //note: this write to LCRH must be after the BRD assignments
  UART1_LCRH_R = 0x60; // got from Bai textbook page 662

  //use system clock as source
  //note from the datasheet UARTCCC register description:
  //field is 0 (system clock) by default on reset
  //Good to be explicit in your code
  UART1_CC_R = 0x00;

  //////Enable interrupts

  //first clear RX interrupt flag (clear by writing 1 to ICR)
  UART1_ICR_R |= 0b00010000;

  //enable RX raw interrupts in interrupt mask register
  UART1_IM_R |= 0b00010000;

  //NVIC setup: set priority of UART1 interrupt to 1 in bits 21-23
  NVIC_PRI1_R = (NVIC_PRI1_R & 0xFF0FFFFF) | 0x00200000;

  //NVIC setup: enable interrupt for UART1, IRQ #6, set bit 6
  NVIC_EN0_R |= 0x40;

  //tell CPU to use ISR handler for UART1 (see interrupt.h file)
  //from system header file: #define INT_UART1 22
  IntRegister(INT_UART1, UART1_Handler);

  //globally allow CPU to service interrupts (see interrupt.h file)
  IntMasterEnable();

  //re-enable UART1 and also enable RX, TX (three bits)
  //note from the datasheet UARTCTL register description:
  //RX and TX are enabled by default on reset
  //Good to be explicit in your code
  //Be careful to not clear RX and TX enable bits
  //(either preserve if already set or set them)
  UART1_CTL_R |= 0b1100000001; // UART1_CTL_R = ??? was the original line, but page 662 of Bai textbook uses what is not commented

}

void uart_sendChar(char data){
    while((UART1_FR_R & 0x20) != 0);
        UART1_DR_R = data;
}

/*char uart_receive(void){
	//DO NOT USE this busy-wait function if using RX interrupt
}*/

void uart_sendStr(const char *data){
    while (*data != '\0') {
            uart_sendChar(*data);
            data++;
        }}

// Interrupt handler for receive interrupts
void UART1_Handler(void)
{
    char byte_received;
    //check if handler called due to RX event
    if (UART1_MIS_R & 0x10)
    {
        //byte was received in the UART data register
        //clear the RX trigger flag (clear by writing 1 to ICR)
        UART1_ICR_R |= 0b00010000;

        //read the byte received from UART1_DR_R and echo it back to PuTTY
        //ignore the error bits in UART1_DR_R
        byte_received = (char)(UART1_DR_R & 0xFF);
        uart_sendChar(byte_received);
        uart_sendChar('\r');
        uart_sendChar('\n');


        currentChar = byte_received;
        //if byte received is a carriage return
        if (byte_received == '\r')
        {
            //send a newline character back to PuTTY
            uart_sendChar('\n');
            uart_sendChar('\r');
        }
        else
        {
            //AS NEEDED
            //code to handle any other special characters
            //code to update global shared variables
            //DO NOT PUT TIME-CONSUMING CODE IN AN ISR

            if (byte_received == command_byte)
            {
              command_flag = 1;
            }
        }
    }
}
