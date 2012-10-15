#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

int read_socket(int fd, char *buffer);
int socketStart(const char* host_name, unsigned int port);

#endif // SOCKET_H_INCLUDED
