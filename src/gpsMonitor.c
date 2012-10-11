#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "/usr/include/gps.h"
#include "gpsMonitor.h"


static struct gps_data_t *gpsdata;
static pthread_t gps_report_handle_thread; //Variavel que aponta a thread.

void gpsReportTask(void){

    //when status is >0, you have data.
    while(gpsdata->status==0){
        //block for up to .5 seconds
        if (gps_waiting(gpsdata, 500)){

            if(gps_read(gpsdata)==-1){
                printf("GPSd read Error\n");
                gpsStop();
                exit(EXIT_FAILURE);
                break;
            }
            else{
                //status>0 means you have data
                if(gpsdata->status>0){
                    //sometimes if your GPS doesnt have a fix, it sends you data anyways
                    //the values for the fix are NaN. this is a clever way to check for NaN.
                    if(gpsdata->fix.longitude!=gpsdata->fix.longitude || gpsdata->fix.altitude!=gpsdata->fix.altitude){
                        printf("Could not get a GPS fix.\n");
                        gpsStop();
                        exit(EXIT_FAILURE);
                    }
                    //otherwise you have a legitimate fix!
                    else{
                        printf("\n");
                        gpsDebugDump(gpsdata);
                        printf("\n");
                    }
                }
                //if you don't have any data yet, keep waiting for it.
                else
                    printf(".");
            }
        }
        //apparently gps_stream disables itself after a few seconds.. in this case, gps_waiting returns false.
        //we want to re-register for updates and keep looping! we dont have a fix yet.
        else
            gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);

        //just a sleep for good measure.
        sleep(1);
    }
    gpsStop();
}

void gpsStart(void){
    //connect to GPSd
    if(gps_open("localhost", "2947", gpsdata)<0){
        printf("Could not connect to GPSd\n");
        exit(EXIT_FAILURE);
    }
    //register for updates
    gps_stream(gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);

    //Create a thread that handle received reports from GPSd.
    pthread_create(&gps_report_handle_thread, NULL,
                   (void * (*)(void *))gpsReportTask,
				   (void*)NULL);

    printf("Waiting for gps lock.");
}

void gpsStop(void){
    //Disable watch
    gps_stream(gpsdata, WATCH_DISABLE, NULL);
    //Close socket with GPSd
    gps_close(gpsdata);
}



void gpsDebugDump(struct gps_data_t *gpsReport){
    printf("Longitude: %lf\nLatitude: %lf\nAltitude: %lf\nAccuracy: %lf\n\n",
                gpsReport->fix.latitude, gpsReport->fix.longitude, gpsReport->fix.altitude,
                (gpsReport->fix.epx>gpsReport->fix.epy)?gpsReport->fix.epx:gpsReport->fix.epy);
}
