/*  keyboard_read2.c
 *
 *  Created on: Aug 26, 2010
 *  	Author: Jan Axelson
 *
 *      Demonstrates how to check keyboard input without blocking and
 *      without using using the ncurses library.
 *      This file, led_control.h, and led_control.c are at www.Lvr.com/beagleboard.htm
 */

// To gain access to the LEDs, run the application with administrative privileges 
// (From the file's directory, use "sudo ./keyboard_read2".)
// Or set up a udev rule as described in led_control.c.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include "../led_control/led_control.h"

int getch();
void initialize_leds();
int kbhit();
void led_control(int led_number, int led_state);
void reset_terminal_attributes();
void set_terminal_attributes();

struct termios original_termios;

/*
 * Checks for pending keypresses, retrieves the keys, and takes appropriate action.
 * The application can perform other tasks in between checking for key presses.
 * Ctrl+C ends the application.
 */

int main(int argc, char *argv[])
{
	int keypress=0;

	led_initialize();
	set_terminal_attributes();

	// Quit on Ctrl+C (keypress = 3).

	while (keypress !=3)
	{
	    while (kbhit())
	    {
			// A key was pressed. Find out which one.

			keypress  = getch();

			// Take action according to the key pressed.

			if (keypress=='0')
				{
					// 0 was pressed. Turn off LED 0.

					led_control(0, 0);
				}
			if (keypress=='1')
				{
					// 1 was pressed. Turn on LED 0.

					led_control(0, 1);
				}
			// Ignore other keys except for Ctrl+C.
	    }

	    // The application can perform other tasks here.
	    // sleep(1) is a place holder for these tasks. (OK to delete.)

	    sleep(1);
	}
    return 0;
}

/*
 * Reads a character from file descriptor zero.
 * Returns the character read or on error, -1.
 */

int getch()
{
    int read_return;
    unsigned char character_read;

    // Read file descriptor 0.

    if ((read_return = read(STDIN_FILENO, &character_read, sizeof(character_read))) < 0)
    {
    	// Error. Return -1.

        return read_return;
    } else
    {
    	// Return the character read.

        return character_read;
    }
}

/*
 * Checks file descriptor 0 for data to read.
 * Returns the number of file descriptors present or on timeout, zero.
 */

int kbhit()
{
	// Set a timeout value of 0 secs, 0 microsecs.

    struct timeval tv = { 0L, 0L };

    // Define a buffer to hold a file descriptor set.

    fd_set fds;

    // Add file descriptor zero to the descriptor set.

    FD_SET(STDIN_FILENO, &fds);

    // Check file descriptor zero in fds for a pending read with zero timeout.
    // The first parameter is the maximum file descriptor number to check + 1.
    // Returns 1 if the descriptor has a pending read or zero on timeout.

    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

/*
 * Resets the terminal to saved values.
 */

void reset_terminal_attributes()
{
	// Returns the terminal's attributes to the saved values.
	// TCSANOW causes the action to take place immediately.

    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

/*
 * Sets up a new terminal with I/O processing disabled so the application can manage it.
 */

void set_terminal_attributes()
{
    struct termios new_termios;

    // Get the current terminal's attributes (file descriptor zero).

    tcgetattr(STDIN_FILENO, &original_termios);

    // Copy the terminal's attributes to the new terminal's structure.

    memcpy(&new_termios, &original_termios, sizeof(new_termios));

    // Register a handler that restores the original terminal's attributes
    // when the process ends.

    atexit(reset_terminal_attributes);

    // Disable all I/O processing.

    cfmakeraw(&new_termios);

    // Set the new terminal's attributes to match the current terminal's attributes.
    // TCSANOW causes the action to take place immediately.

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}
