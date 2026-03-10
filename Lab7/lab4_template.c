/**
 * lab4_template.c
 *
 * Template file for CprE 288 lab 4
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 */

#include "button.h"
#include "Timer.h"
#include "lcd.h"
#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1
#include "mission1.h"




/*int main(void) {
	button_init();
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
    cyBot_uart_init(); // Don't forget to initialize the cyBot UART before trying to use it

	// YOUR CODE HERE

    uint8_t buttonPressed;
    uint8_t lastButtonPressed;
    char temp[100];
	
	while(1){
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
*/
