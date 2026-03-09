/*
*
*   uart.c
*
*
*
*   @author
*   @date
*/

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "uart.h"

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

void uart_sendChar(char data){
	while((UART1_FR_R & 0x20) != 0);
    UART1_DR_R = data;

	}

  char uart_receive(void){
    while ((UART1_FR_R & 0x10) != 0);
    return (char) (UART1_DR_R & 0xFF);

}

void uart_sendStr(const char *data) {
    while (*data != '\0') {
        uart_sendChar(*data);
        data++;
    }
}


