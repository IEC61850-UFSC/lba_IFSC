#ifndef SERIALPORT_H_INCLUDED
#define SERIALPORT_H_INCLUDED

#define MAX_COMMAND_LENGTH 10

void serial_port_close(int serial_port);
int serial_port_open(void);
int serial_port_read(char *read_buffer, size_t max_chars_to_read, int serial_port);
void serial_port_write(char *write_buffer, int serial_port);
void sigint_handler(int sig, int serial_port);
void onCommand(char *read_buffer, int chars_read, int serial_port);
void serialPortStart(void);

#endif // SERIALPORT_H_INCLUDED
