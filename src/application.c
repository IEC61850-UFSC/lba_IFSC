#include <stdlib.h>
#include "include/leds.h"
#include "include/serialPort.h"

void applicationStart(void){
}

void applicationRun(void){
}

void applicationStop(int serial_port){
    led_control(0, 0);
    led_control(1, 0);
    serial_port_close(serial_port);
    exit(EXIT_SUCCESS);
}
