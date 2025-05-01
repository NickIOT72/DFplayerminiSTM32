#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "moduleInit.h"

void Serial_write(UART_HandleTypeDef *huart , uint8_t* buffer, uint8_t len);
void Serial_print( UART_HandleTypeDef *huart , const char *fmt, ...);
void Serial_pushData(uint8_t* data, uint8_t size);
uint8_t Serial_read();
bool Serial_available();

#endif