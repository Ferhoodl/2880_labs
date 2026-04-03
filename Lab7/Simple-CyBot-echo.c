///////////////////////////////////////////////////////////////////////
// Name: Simple-CyBot-echo.c                                         //
// Author: Phillip Jones (phjones@iastate.edu)                       //
// Date: 09/26/2023                                                  //
// Description: Simple Cybot program that is an Echo'ish program for //
//              communicating with the Starter Client Socket code.   //
///////////////////////////////////////////////////////////////////////


#include "open_interface.h"
#include "lcd.h"
#include "stdio.h"
#include "math.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "mission2.h"
#include "timer.h"
#include "uart-interrupt.h"
#include "Timer.h"
#include "lcd.h"
#include "cyBot_uart.h"
extern volatile char currentChar;
extern volatile char currentCharIsFresh;
int main (void) {

  timer_init(); // Initialize Timer, needed before any LCD screen fucntions can be called 
	              // and enables time functions (e.g. timer_waitMillis)
  //lcd_init();   // Initialize the the LCD screen.  This also clears the screen.
  //cyBot_uart_init();  // Initialize UART

  oi_t *sensor_data = oi_alloc();
  uart_interrupt_init();   // Part 3: use our own UART, not cyBot_uart_init()
  oi_init(sensor_data);
  timer_init();
  lcd_init();
  cyBOT_init_Scan(0b0111);
  oi_setWheels(0, 0);

  char my_data;       // Variable to get bytes from Client
  char command[100];  // Buffer to store command from Client
  int index = 0;      // Index position within the command buffer

  // Write to LCD so that we know the program is running
  lcd_printf("Running");

  right_calibration_value = 222250; // bot 01
  left_calibration_value  = 1198750;


  rawScannerDatas rawDatas;

  while(1)
  {

    index = 0;  // Set index to the beginning of the command buffe
    while(!currentCharIsFresh){}

    if(currentChar == 's'){
        scanField2(&rawDatas);
    }

    currentCharIsFresh = 0;
/*
    // Get the rest of the command until a newline byte (i.e., '\n') received
    while(currentChar != '\n')
    {
      command[index] = currentChar;  // Place byte into the command buffer
      currentCharIsFresh = 0;
      index++;
    }

    command[index] = '\n';  // place newline into command in case one wants to echo the full command back to the Client
    command[index+1] = 0;   // End command C-string with a NULL byte so that functions like printf know when to stop printing

    lcd_printf("Got: %s", command);  // Print received command to the LCD screen

    // Send a response to the Client (Starter Client expects the response to end with \n)
    // In this case I am just sending back the first byte of the command received and a '\n'
    cyBot_sendByte(command[0]);

    // Only send a '\n' if the first byte of the command is not a '\n',
    // to avoid sending back-to-back '\n' to the client
    if(command[0] != '\n')
    {
      cyBot_sendByte('\n');
    }

    */
  }
 
  return 0;
}

