/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */
/*
#include "Timer.h"
#include "lcd.h"


int main (void) {

	timer_init(); // Initialize Timer, needed before any LCD screen functions can be called 
	              // and enables time functions (e.g. timer_waitMillis)

	lcd_init();   // Initialize the LCD screen.  This also clears the screen. 

	char referenceString[106] = "                    Microcontrollers are lots of fun!                    Microcontrollers are lots of fun!";
	char clippedString[20] = "";
	int beginStringClip;
	int messageLength = 20;

	int wrapCounter = 0;
	int i;
	while(1){
	    if(wrapCounter == 86){
	        wrapCounter = 33;
	    }
	    beginStringClip = wrapCounter;
	    for(i = 0; i < messageLength; i++){
	        clippedString[i] = referenceString[beginStringClip + i];
	    }
	    // Print "Hello, world" on the LCD
	    lcd_printf(clippedString);

	    wrapCounter++;
	    timer_waitMillis(300);
	}

	// lcd_puts("Hello, world"); // Replace l
	 cd_printf with lcd_puts
        // step through in debug mode and explain to TA how it works
    
	// NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
       // NOTE: For time functions, see Timer.h

	return 0;
}
*/
