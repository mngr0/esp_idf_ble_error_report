#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stddef.h>

void serial_configure();

int serial_send_buffer(uint8_t* data, size_t lenght);

#endif