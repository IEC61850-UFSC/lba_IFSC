#ifndef GPSMONITOR_H_INCLUDED
#define GPSMONITOR_H_INCLUDED

#define GPSD_HOST_NAME "localhost"
#define GPSD_PORT 2947

void gpsStart(void);
void gpsStop(void);

#endif // GPSMONITOR_H_INCLUDED
