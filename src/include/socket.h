#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED


#pragma pack(1)

typedef struct{
    char ini;
    char tag[8];
    char header_class[10];
    char rest[79];
    //DWORD img_size;
}GPSD_PROTOCOL_VERSION_HEADER;

typedef struct{
    char ini;
    char tag[8];
    char header_class[10];
    char rest[51];
    //DWORD img_size;
}GPSD_PROTOCOL_DEVICE_HEADER;

int socketStart(const char* server_name, int server_port, char *des_path);

#endif // SOCKET_H_INCLUDED
