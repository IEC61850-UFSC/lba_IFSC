#include <unistd.h>
#include "include/serialPort.h"
#include "include/leds.h"
#include "include/gpsMonitor.h"

int main(void){

    serialPortStart();
    gpsStart();

    for (;;)
	{
		led_control(1, 1);
		sleep(1);
		led_control(1, 0);
		sleep(1);
	}
	return 0;
}
