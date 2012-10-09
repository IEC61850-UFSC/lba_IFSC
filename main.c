#include <unistd.h>
#include "serialPort.h"
#include "leds.h"

int main(void){

    serialPortStart();

    for (;;)
	{
		led_control(1, 1);
		sleep(1);
		led_control(1, 0);
		sleep(1);
	}
	return 0;
}
