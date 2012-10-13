#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "include/gps.h"
#include "include/gpsMonitor.h"
#include "include/socket.h"

static struct gps_data_t *gpsdata;
//static pthread_t gps_report_handle_thread; //Variavel que aponta a thread.

void gpsStart(void){

    //Start Socket.
    socketStart(GPSD_HOST_NAME, GPSD_PORT, "HOLA");

    //Enable Watch.
    printf("Waiting for gps lock.\n");
}

void gpsStop(void){
    //Disable watch
    gps_stream(gpsdata, WATCH_DISABLE, NULL);
    //Close socket with GPSd
    gps_close(gpsdata);

    //pthread_cancel(gps_report_handle_thread);
    //pthread_join(gps_report_handle_thread, NULL);
}
