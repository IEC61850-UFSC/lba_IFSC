#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "include/gpsMonitor.h"
char PATH[] =  "/home/ubuntu/videos/";
GPS_LAT_LON waypooints[] = {
                            {-27.5842, -48.5226},
                            {-27.5812, -48.5232}
                            };

void videoPlay(int lat, int lon){
    char vlc_call[255], video_to_play[255];

    sprintf(video_to_play,"BeagleBoard-xM.flv");
    //Determine with video to play from the Video Path
    sprintf(vlc_call,"/usr/bin/vlc --play-and-exit %s%s",PATH,video_to_play);
    printf("vlc_call = %s\n",vlc_call);
    system(vlc_call);
}

/*
void videoGet(void){
}
*/
