/*
 * led_control.c
 *
 *  Created on: Aug 26, 2010
 *  	Author: Jan Axelson
 *
 *      Functions to initialize and control the usr0 and usr1 LEDs
 *      on the BeagleBoard.
 *      www.Lvr.com/beagleboard.htm
 
By default, the OS requires administrative privileges to control the LEDs. A udev rule can grant user privileges to control the LEDs. 

Create a file called 90-leds.rules in /etc/udev/rules.d/ and copy this rule into it:

SUBSYSTEM=="leds", PROGRAM+="/bin/sh -c 'chmod a+rw /sys/devices/platform/leds-gpio/leds/beagleboard::usr0/trigger /sys/devices/platform/leds-gpio/leds/beagleboard::usr0/brightness /sys/devices/platform/leds-gpio/leds/beagleboard::usr1/trigger /sys/devices/platform/leds-gpio/leds/beagleboard::usr1/brightness'"

The rule grants read/write permissions to all users for the trigger and brightness attributes for both LEDs.
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Turns an LED on or off.
 * led_number: 0 = usr0, 1 = usr1.
 * led_state: 0 = off, 1 = on.
 * Each LED has a brightness file whose contents control the LED's state.
  */

void led_control(int led_number, int led_state)
{
	char ch_led_state[2];
	char* led_brightness[2];
	FILE *p_led_control = NULL;

	led_brightness[0] = "/sys/class/leds/beagleboard::usr0/brightness";
	led_brightness[1] = "/sys/class/leds/beagleboard::usr1/brightness";

	if ((p_led_control = fopen(led_brightness[led_number], "r+")) != NULL)
	{
		// The specified LED's brightness file is open for reading and writing.
		// Convert led_state to a char, write the result to the brightness file,
		// and close the file.

		sprintf(ch_led_state, "%d", led_state);
		fwrite(ch_led_state, sizeof(char), 1, p_led_control);
		fclose(p_led_control);
	}
}

/*
 * Disables any active kernel LED functions by writing "none"
 * to the trigger files for the usr0 and usr1 LEDs.
 * (Writing "0" to the LED's brightness file also seems to set the trigger to "none".)
 */

void led_initialize()
{
	int led_number;
	char* led_trigger[2];
	led_trigger[0] = "/sys/class/leds/beagleboard::usr0/trigger";
	led_trigger[1] = "/sys/class/leds/beagleboard::usr1/trigger";
	FILE *p_led_trigger = NULL;
	char trigger_none[] = "none";

	for (led_number = 0; led_number < 2; led_number++)
	{
		if ((p_led_trigger = fopen(led_trigger[led_number], "r+")) != NULL)
		{
			// The specified LED's trigger file is open for reading and writing.
			// Write "none" to the trigger file and close the file.

			fwrite(trigger_none, sizeof(trigger_none), sizeof(char), p_led_trigger);
			fclose(p_led_trigger);

		}
	}
	// Turn off both LEDs.

	led_control(0,0);
	led_control(1,0);

}
