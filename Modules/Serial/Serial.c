#include "Serial.h"

#define BUFFER_LENGTH 256
uint8_t RX_data[BUFFER_LENGTH];
uint16_t rx_data_position= 0;
uint16_t rx_counter = 0;
uint16_t rx_counter_data_position =0;

bool Serial_available()
{
	return rx_data_position > 0;
}

uint8_t Serial_read()
{
	if (rx_data_position == 0) return 0;
	uint8_t ret_data = RX_data[rx_counter%BUFFER_LENGTH];
	RX_data[rx_counter%BUFFER_LENGTH] = 0;
	rx_counter += 1;
	if ( rx_counter == rx_counter_data_position )
	{
		rx_counter = 0;
		rx_data_position = 0;
		rx_counter_data_position = 0;
	}
	return ret_data;
}

void Serial_pushData(uint8_t* data, uint8_t size)
{
	uint8_t startByte = 0;
	for ( int i = 0; i < size; i++ )
	{
		if ( data[i] == 0x7E ){
			startByte = i;
			break; 
		}
	}
	for ( int i = 0; i < size; i++ )
	{
		RX_data[rx_data_position] = data[ (startByte+i)%size];
		rx_data_position += 1;
		rx_counter_data_position += 1;
		rx_data_position %= BUFFER_LENGTH;
	}
}

void Serial_write(UART_HandleTypeDef *huart , uint8_t* buffer, uint8_t len)
{
	HAL_UART_Transmit(huart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
}

void Serial_print(UART_HandleTypeDef *huart ,const char *fmt, ...)
{
  static char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  int len = strlen(buffer);
  HAL_UART_Transmit(huart, (uint8_t*)buffer, len, HAL_MAX_DELAY);
}
