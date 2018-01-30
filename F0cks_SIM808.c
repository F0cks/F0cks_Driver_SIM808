#include "F0cks_SIM808.h"

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config)
{
	int8_t error = 0;
	uint8_t i = 0;
	char *p;

	/* Get UART Circular Buffer data from user */
	handler->uartCircularBuffer     = config.uartCircularBuffer;
	handler->uartCircularBufferSize = config.uartCircularBufferSize;

	/* Use private pointer to read UART data */
	handler->privateCircularBufferP = handler->uartCircularBuffer;
	/* Clean and prepare circular buffer */
	p = config.uartCircularBuffer;
	for(i=0; i<handler->uartCircularBufferSize; i++ )
	{
		*p = '\200';
		p++;
	}


	return error;
}

/* Power ON SIM808 using PWRKEY */
void F0cks_SIM808_Power_ON()
{
	F0cks_SIM808_PWRKEY_High();
	F0cks_Delay_ms(500);
	F0cks_SIM808_PWRKEY_Low();
	F0cks_Delay_ms(1000);
	F0cks_SIM808_PWRKEY_High();
}

/* Power OFF SIM808 using PWRKEY */
void F0cks_SIM808_Power_OFF()
{
	F0cks_SIM808_PWRKEY_Low();
	F0cks_Delay_ms(2000);
	F0cks_SIM808_PWRKEY_High();
}
