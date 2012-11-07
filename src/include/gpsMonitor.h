#ifndef GPSMONITOR_H_INCLUDED
#define GPSMONITOR_H_INCLUDED

#define GPSD_HOST_NAME "localhost"
#define GPSD_PORT 2947
#define MAX_GPSD_MSG_LENGHT 1500

typedef struct{
    double lat;
    double lon;
}GPS_LAT_LON;

extern GPS_LAT_LON car_position;
void gpsStart(void);
void gpsStop(void);

#endif // GPSMONITOR_H_INCLUDED
