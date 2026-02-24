/**
 * lab5_template.c
 *
 * Template file for CprE 288 Lab 5
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 *
 * @author Phillip Jones, updated 6/4/2019
 * @author Diane Rover, updated 2/25/2021, 2/17/2022
 *
 * Authors: Mihir Mistry, Isaiah Steele 02/24/2026
 */

#include "button.h"
#include "timer.h"
#include "lcd.h"
#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART1)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1
#include "cyBot_Scan.h"  // Scan using CyBot servo and sensors
#include "mission1.h"

int main(void) {
	button_init();
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();

  // initialize the cyBot UART1 before trying to use it
  // (Uncomment ME for UART init part of lab)
  //cyBot_uart_init_clean();  // Clean UART1 initialization, before running your UART1 GPIO init code

  // Complete this code for configuring the GPIO PORTB part of UART1 initialization (your UART1 GPIO init code)
      SYSCTL_RCGCGPIO_R |= 0x02;                      // turn on port B
	  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};       // wait until port b is confirmed to be on
	  GPIO_PORTB_DEN_R |= 0x03;                       // enable alternate function
	  GPIO_PORTB_AFSEL_R |= 0x03;                     // alternate function select on two lines
      GPIO_PORTB_PCTL_R &= 0xFFFFFF00;                     // Force 0's in the desired locations
      GPIO_PORTB_PCTL_R |= 0x00000011;                     // Force 1's in the desired locations

	  // Or see the notes for a coding alternative to assign a value to the PCTL field

      // (Uncomment ME for UART init part of lab)
		// cyBot_uart_init_last_half();  // Complete the UART device configuration

	   // Initialize the scan
	     cyBOT_init_Scan(0b0111);
		// Remember servo calibration function and variables from Lab 3
	     right_calibration_value = 290500;
	     left_calibration_value = 1288000;

	while(1)
	{
	    uint8_t buttonPressed;
	    uint8_t lastButtonPressed;
        char temp[50] = 0;

        buttonPressed = button_getButton();
        if(lastButtonPressed != buttonPressed){
            if(buttonPressed > 0){
                lcd_clear();
                lcd_printf("Button %d pressed", buttonPressed);

                sprintf(temp, "Button %d is pressed\n\r", buttonPressed);
                sendMessage(temp);
            }else{
                lcd_clear();
                lcd_printf("No button pressed.");

                sprintf(temp, "No button pressed.\n\r");
                sendMessage(temp);
            }

            lastButtonPressed = buttonPressed;
        }
	}
}
