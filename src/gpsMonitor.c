#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
//#include "include/gps.h"
#include "include/gpsMonitor.h"
#include "include/socket.h"
#include "include/serialPort.h"
#include "include/application.h"
#include "include/video.h"

#define TIMEOUT 30

struct gps_socket_arg {    /* strutura de argumentos para a thread */
    int      fd;                 /* file descriptor do socket */
    char*    destination_path;   /* pasta onde vão ser salvadas as imagens */
};

GPS_LAT_LON car_position;
static pthread_t gps_report_handle_thread; //Variavel que aponta a thread.
static char gps_msg[MAX_GPSD_MSG_LENGHT];
static int gps_socket_fd;
time_t start, end;

#if 1
//Thread que lee o socket e caso uma imagem seja capturada a salva
//na pasta especificada pelo parametro arg->destination_path+indicie.
int gpsMessageTask(void *arg){

    int n,i,j; //Variaveis auxiliares.
    //double int_lat,int_lon;
    char *w_buffer;
    char lat[15],lon[15];
    struct gps_socket_arg *args = (struct gps_socket_arg  *)arg;  //Structura onde estão os argumentos da função.

    //Reading Version Tag
    n = read_socket(args->fd,gps_msg);
    //printf("%s\n",gps_msg);

    //Enable to visualize gpsd data
    w_buffer = "?WATCH={\"enable\":true,\"json\":true}";
    //"?WATCH={"enable":true,"json":true}"
	write(args->fd,w_buffer,strlen(w_buffer));

	for(;;)
	{
        n = read_socket(args->fd,gps_msg);

        if(gps_msg[10]=='T' && gps_msg[11]=='P' && gps_msg[12]=='V'){
            for(i = 92; i <=104;i++){
                lat[i-92]=gps_msg[i];
            }
            lat[14]='\0';
            for(i = 112; i <=124;i++){
                lon[i-112]=gps_msg[i];
            }
            lon[14]='\0';
            car_position.lat = atof(lat);
            car_position.lon = atof(lon);

            end = time(NULL);
            //Play the video if necessary
            if((difftime(end,start)) > TIMEOUT){
                videoPlay(car_position.lat,car_position.lon);
                start = time (NULL);
            }

            //printf("lat =%s\nlon =%s\n",lat,lon);
            //printf ("%ld hours since January 1, 1970\n", start);

            //printf("int_lat =%lf\nint_lon =%lf\n",car_position.lat,car_position.lon);
        }
        else if(gps_msg[10]=='D' && gps_msg[11]=='E' && gps_msg[12]=='V'){
            if(gps_msg[52] =='0'){
                printf("GPSD FOI DESATIVADO = %c\n",gps_msg[52]);
                break;
            }
        }
        sleep(1);
        j++;
        if (j>500){
            w_buffer = "?WATCH={\"enable\":true,\"json\":true}";
            j = 0;
        }
	}
    applicationStop(serial_port);
	return 0;
}
#endif

void gpsStart(void){
    struct gps_socket_arg *gps_thread_arguments;

    gps_socket_fd = socketStart(GPSD_HOST_NAME, GPSD_PORT);
    //Start GPS Socket.
    gps_thread_arguments = calloc(1, sizeof(struct gps_socket_arg));
    gps_thread_arguments[0].fd = gps_socket_fd;
    gps_thread_arguments[0].destination_path = "HOLA";

    //gps_msg = (char*)malloc(MAX_GPSD_MSG_LENGHT);

    start = time (NULL);
    // Cria uma thread para monitorar a conexão
	pthread_create(&gps_report_handle_thread, NULL,
                   (void * (*)(void *))gpsMessageTask,
                   &gps_thread_arguments[0]);

    //Enable Watch.
    printf("Waiting for gps lock.\n");
}

void gpsStop(void){
    char *w_buffer;
    printf("GPS STOP\n");
    //Disable watch
    w_buffer = "?WATCH={\"enable\":false}";
    //?WATCH={"enable":false}
	write(gps_socket_fd,w_buffer,strlen(w_buffer));
	//Close socket with GPSd
	close(gps_socket_fd);
	//free(gps_msg);
	system("killall -9 gpsd");
}
