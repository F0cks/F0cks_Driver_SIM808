#include "F0cks_SIM808.h"
#include "usart.h"

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config)
{
	int8_t error = 0;
	uint8_t i = 0;
	uint8_t *p;

	/* Get UART Circular Buffer data from user */
	handler->uartCircularBuffer     = config.uartCircularBuffer;
	handler->uartCircularBufferSize = config.uartCircularBufferSize;

	/* Get pin and apn */
	while( (*config.pinCode != '\0') && (i < PIN_SIZE))
	{
		handler->pinCode[i] = *config.pinCode;
		i++; config.pinCode++;
	}
	i = 0;
	while( (*config.apn != '\0') && (i < APN_SIZE) )
	{
		handler->apn[i] = *config.apn;
		i++; config.apn++;
	}

	/* Use private pointer to read UART data */
	handler->privateCircularBufferP = handler->uartCircularBuffer;
	/* Clean and prepare circular buffer */
	p = handler->uartCircularBuffer;
	for(i=0; i<handler->uartCircularBufferSize; i++ )
	{
		*p = '\200';
		p++;
	}

	for(i=0; i< STRING_BUFFER_SIZE; i++)
	{
		handler->privateStringBuffer[i] = '\0';
	}

	return error;
}

/* Power ON SIM808 using PWRKEY */
void F0cks_SIM808_Power_ON(SIM808_HandleTypeDef *handler)
{
	/* Start module with PWRKEY */
	F0cks_SIM808_PWRKEY_High();
	F0cks_Delay_ms(500);
	F0cks_SIM808_PWRKEY_Low();
	F0cks_Delay_ms(1000);
	F0cks_SIM808_PWRKEY_High();

	/* Initialize UART communication */
	F0cks_SIM808_UART_Send("AT\n\r");// Get first dummy answer
	F0cks_Delay_ms(2000);
	while( *handler->privateCircularBufferP == '\200')// Set correctly private pointer on circular buffer if offset
	{
		handler->privateCircularBufferP++;
	}
	F0cks_SIM808_UART_Send("AT\n\r");
	while(F0cks_SIM808_Check_Ack(handler) != 1);
	while( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "+CPIN: SIM PIN") != 1 )
	{
		F0cks_SIM808_Read_Circular_Buffer(handler);
	}
}

/* Power OFF SIM808 using PWRKEY */
void F0cks_SIM808_Power_OFF(SIM808_HandleTypeDef *handler)
{
	uint8_t i = 0;
	uint8_t *pt = handler->uartCircularBuffer ;

	/* Stop module with PWRKEY */
	F0cks_SIM808_PWRKEY_Low();
	F0cks_Delay_ms(2000);
	F0cks_SIM808_PWRKEY_High();
	F0cks_Delay_ms(500);

	/* Purge circular buffer */
	for(i=0; i<handler->uartCircularBufferSize; i++ )
	{
		*pt++ = '\200';
	}
}

/* Read Circular buffer */
int8_t F0cks_SIM808_Read_Circular_Buffer(SIM808_HandleTypeDef *handler)
{
	uint8_t timeout = 0;
	int8_t  i = 0;
	char    *p = handler->privateStringBuffer;
	uint8_t currentChar = '\0',
	        lastChar    = '\0';

	for(i=0; i< STRING_BUFFER_SIZE; i++)
	{
		handler->privateStringBuffer[i] = '\0';
	}

	while(timeout <= 100)
	{
		if( *handler->privateCircularBufferP == '\0' )
		{
			handler->privateCircularBufferP = handler->uartCircularBuffer;
		}
		if( *handler->privateCircularBufferP != '\200' )
		{
			/* Reset timeout */
			timeout = 0;

			/* Store current and last char */
			lastChar    = currentChar;
			currentChar = *handler->privateCircularBufferP;

			/* Do not store \n and \r */
			if( *handler->privateCircularBufferP != '\n' &&
					*handler->privateCircularBufferP != '\r' &&
					*handler->privateCircularBufferP != 0xff )
			{
				*p = *handler->privateCircularBufferP;
				p++;
			}

			/* Move to next char */
			*handler->privateCircularBufferP = '\200';
			handler->privateCircularBufferP++;

			if(currentChar == '\n' && lastChar == '\r')
			{
				/* New string */
				return 1;
			}
		}
		else
		{
			/* Increase timeout */
			timeout++;
			F0cks_Delay_ms(10);
		}
	}

	/* Timeout: No new string */
	return 0;
}

/* Compare 2 strings */
int8_t F0cks_SIM808_Compare_Strings(char *str1, char *str2)
{
	char *s1 = str1;
	char *s2 = str2;

	/* While string is not fully parsed  */
	while( (*s1 != '\0') && (*s2 != '\0') )
	{
		/* If char are not the same */
		if( *s1 != *s2 )
		{
			return 0;
		}
		s1++; s2++;
	}

	if( *s1 == *s2 )
	{
		/* Strings are the same */
		return 1;
	}
	else
	{
		/* Strings are not the same */
		return 0;
	}
}

int8_t F0cks_SIM808_Check_Ack(SIM808_HandleTypeDef *handler)
{
	/* While there are words in buffer */
	while( F0cks_SIM808_Read_Circular_Buffer(handler) != 0)
	{
		if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "OK") )
		{
			/* ACK OK received */
			return 1;
		}
		else if ( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "NOK") )
		{
			/*ACK NOK received */
			return 0;
		}
	}
	/* No ACK received */
	return -1;
}

int8_t F0cks_SIM808_GSM_Start(SIM808_HandleTypeDef *handler)
{
	/* Set PIN code */
	F0cks_SIM808_UART_Send("AT+CPIN=\"");
	F0cks_SIM808_UART_Send(handler->pinCode);
	F0cks_SIM808_UART_Send("\"\n\r");
	if( F0cks_SIM808_Check_Ack(handler) != 1 )
	{
		return -1;
	}
	while( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "SMS Ready") != 1 )
	{
		F0cks_SIM808_Read_Circular_Buffer(handler);
	}

	/* SMS Format */
	F0cks_SIM808_UART_Send("AT+CMGF=1\n\r");
	if( F0cks_SIM808_Check_Ack(handler) != 1 )
	{
		return -2;
	}
	F0cks_SIM808_UART_Send("AT+CSCS=\"GSM\"\n\r");
	if( F0cks_SIM808_Check_Ack(handler) != 1 )
	{
		return -3;
	}

	/* Set white list */
	F0cks_SIM808_UART_Send("AT+CWHITELIST=1\n\r");
	while( F0cks_SIM808_Check_Ack(handler) != 1 );

	return 1;
}

