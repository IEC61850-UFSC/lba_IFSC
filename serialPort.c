/*
 * usb_serial_port.c
 *
 *  Created on: Sept 13, 2011
 *      Author: Jan Axelson
 */

// Demonstrates communications on a USB virtual serial port.
// Tested with a USB/serial-port adapter that contains an FT232B USB UART.

// Use a null-modem connection to attach the port to a physical or virtual
// serial port on a PC. On the PC, use any terminal emulator to
// communicate with the port at 9600 8-N-1, no flow control.

// To turn on the usr0 LED on the BeagleBoard, type led1on and press <Enter>.
// To turn off the usr0 LED on the BeagleBoard, type led1off and press <Enter>.
// Ctrl+C or a "closeapp" command terminates the program.

// (The only USB-specific code is naming ttyUSB0 as the port.
// Everything else is identical to accessing a physical serial port.)

// To compile the application, copy led_control.h and led_control.c
// to the subdirectory led_control.

// To gain access to the LEDs, run the application with administrative privileges
// (From the file's directory, use "sudo ./usb_serial_port".)
// Or set up a udev rule as described in led_control.c.

// This file, led_control.h, and led_control.c are available at
// www.Lvr.com/beagleboard.htm

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "leds.h"
#include "serialPort.h"
#include "application.h"

static const char *PORT_NAME = "/dev/ttyO2";
static pthread_t message_handle_thread; //Variavel que aponta a thread.
static int serial_port;

static struct termios options_original;


void serialMessageTask(void){
    int chars_read;
    char read_buffer[MAX_COMMAND_LENGTH + 1] = {0};

    for (;;)
	{
		if (serial_port != -1)
		{
			// Read received data into the buffer
			// up to but not including the buffer's terminating null.

			chars_read = serial_port_read(read_buffer, MAX_COMMAND_LENGTH,serial_port);
			if (chars_read > 0)
			{
				// Data was read.
				onCommand(read_buffer, chars_read, serial_port);
			}
		}
		// The application can perform other tasks here.
	}
}

// Resets the terminal and closes the serial port.
void serial_port_close(int serial_port)
{
	tcsetattr(serial_port,TCSANOW,&options_original);
	close(serial_port);
}

// Opens a USB virtual serial port at ttyUSB0.
//
// returns - the port's file descriptor or -1 on error.
int serial_port_open(void)
{
  struct termios options;
  int serial_port;
  serial_port = open(PORT_NAME, O_RDWR | O_NONBLOCK);

  if (serial_port != -1)
  {
	  printf("Serial Port open\n");
	  tcgetattr(serial_port,&options_original);
 	  tcgetattr(serial_port, &options);
	  cfsetispeed(&options, B115200);
	  cfsetospeed(&options, B115200);
	  options.c_cflag |= (CLOCAL | CREAD);
	  options.c_lflag |= ICANON;
	  tcsetattr(serial_port, TCSANOW, &options);
  }
  else
	  printf("Unable to open /dev/ttyUSB0\n");
  return (serial_port);
}

// Attempts to read up to 10 bytes from the serial port.
// If data was read, calls a routine to examine the received data
// and take action.
// *read_buffer - the buffer that will contain the data read.
// max_chars_to_read - the maximum number of characters to read.
// returns - 0 if data was received, -1 if no data received.
int serial_port_read(char *read_buffer, size_t max_chars_to_read, int serial_port)
{
	int chars_read = read(serial_port, read_buffer, max_chars_to_read);

	return chars_read;
}

// Writes data to the port.
// Parameter:  write_buffer - the data to write to the port.
// *write_buffer - the buffer that contains the data to write.
void serial_port_write(char *write_buffer, int serial_port)
{
	int bytes_written;
	size_t len = 0;

	len = strlen(write_buffer);
	bytes_written = write(serial_port, write_buffer, len);
	if (bytes_written < len){
		printf("Write failed \n");
	}
}

// Executes when the user presses Ctrl+C.
// Closes the port, resets the terminal, and exits the program.
void  sigint_handler(int sig, int serial_port)
{
	serial_port_close(serial_port);
	exit (sig);
}

// Looks for defined commands and takes appropriate action.
// read_buffer - the data to examine.
void onCommand(char *read_buffer, int chars_read, int serial_port)
{
	// Terminate the data read with a null to enable using string functions
	// on the data read.

	if (strstr(read_buffer, "led1on") != NULL)
	{
		led_control(0, 1);
		serial_port_write("led1 is on\r\n",serial_port);
		printf("led1 is on\r\n");
	}
	else if (strstr(read_buffer, "led1off") != NULL)
	{
		led_control(0, 0);
		serial_port_write("led1 is off\r\n",serial_port);
		printf("led1 is off\r\n");
	}
	else if (strstr(read_buffer, "closeapp") != NULL)
	{
		applicationStop(serial_port);
	}
	else
	{
		// Ignore undefined commands.
	}
}

void serialPortStart(void){
    //enable system to write on leds
    led_initialize();
    //enable system to write and read serial port (/dev/ttyO2)
	serial_port = serial_port_open();

	//Create a thread that handle received commands.
    pthread_create(&message_handle_thread, NULL,
                        (void * (*)(void *))serialMessageTask,
						(void*)NULL);

	if (serial_port != -1)
	{
		// Assign a handler to close the serial port on Ctrl+C.

		signal (SIGINT, (void*)sigint_handler);

		serial_port_write("USB virtual serial port test program\r\n",serial_port);
		serial_port_write("To control the usr0 LED, type led1on or led1off and press <Enter>\r\n",serial_port);
		serial_port_write("To end the remote application, type closeapp and press <Enter> \r\n",serial_port);

		printf("Waiting to receive commands...\n");
		printf("Press Ctrl+C to exit the program.\n");

	}
}





