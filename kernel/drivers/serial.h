#pragma once

#include <types.h>

START_EXTERNC

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define SERIAL_ENABLE_DLAB 0x80
#define SERIAL_RATE_38400_LO 0x03
#define SERIAL_RATE_38400_HI 0x00
#define SERIAL_BUFFER_EMPTY 0x20

/**
 * @brief Initialize the serial port
 * 
 * @param serial_port 
 * @return 0 or -1 if the serial port is faulty.
 */
int init_serial(int serial_port);
int serial_received(int serial_port);
char read_serial(int serial_port);
int is_transmit_empty(int serial_port);
void write_serial(int serial_port, char a);
/**
 * @brief Sends a text to the serial port (Used by the debug system, don't use this!!)
 * 
 * @param serial_port 
 * @param text 
 */
void serial_write_text(int serial_port, char *text);

END_EXTERNC
