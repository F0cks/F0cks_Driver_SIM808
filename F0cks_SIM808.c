#include "F0cks_SIM808.h"
#include "usart.h"

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config)
{
	int8_t error = 0;
	uint8_t i = 0;
	uint8_t *p;

	/* Battery */
	handler->battery = (SIM808_BatteryTypeDef){0,0,0};

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
		/* Reset pointer */
		if( *handler->privateCircularBufferP == '\0' )
		{
			handler->privateCircularBufferP = handler->uartCircularBuffer;
		}
	}
	F0cks_SIM808_UART_Send("AT\n\r");
	F0cks_Delay_ms(1000);
	while(F0cks_SIM808_Parse_String(handler) != 1); // OK
	while(F0cks_SIM808_Parse_String(handler) != 4); // +CPIN: SIM PIN
	F0cks_SIM808_Battery_Update(handler);
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

/* Compare 1 string with a pattern */
int8_t F0cks_SIM808_Compare_Strings(char *str1, char *str2)
{
	char *s1 = str1;
	char *s2 = str2;

	/* Till 1 string is fully parsed  */
	while( (*s1 != '\0') && (*s2 != '\0') )
	{
		/* If char are not the same */
		if( *s1 != *s2 )
		{
			return 0;
		}
		s1++; s2++;
	}

	if( *--s1 == *--s2 )
	{
		/* String contains pattern */
		return 1;
	}
	else
	{
		/* String does not contain pattern */
		return 0;
	}
}

/* Enable GSM */
void F0cks_SIM808_GSM_Start(SIM808_HandleTypeDef *handler)
{
	/* Set PIN code */
	F0cks_SIM808_UART_Send("AT+CPIN=\"");
	F0cks_SIM808_UART_Send(handler->pinCode);
	F0cks_SIM808_UART_Send("\"\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK
	while( F0cks_SIM808_Parse_String(handler) != 5 ); // != SMS Ready

	/* SMS Format */
	F0cks_SIM808_UART_Send("AT+CMGF=1\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK

	F0cks_SIM808_UART_Send("AT+CSCS=\"GSM\"\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK

	/* Set white list */
	F0cks_SIM808_UART_Send("AT+CWHITELIST=1\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK
}

/* Parse a string */
int8_t F0cks_SIM808_Parse_String(SIM808_HandleTypeDef *handler)
{
	/* Get string */
	if(F0cks_SIM808_Read_Circular_Buffer(handler) == 0)
	{
		/* Nothing to parse */
		return 0;
	}

	/* Parse string */
	if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "OK") )
	{
		return 1;
	}
	else if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "NOK") )
	{
		return 2;
	}
	else if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "+CBC:") )
	{
		return 3;
	}
	else if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "+CPIN: SIM PIN") )
	{
		return 4;
	}
	else if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "SMS Ready") )
	{
		return 5;
	}
	else if( F0cks_SIM808_Compare_Strings(handler->privateStringBuffer, "ERROR") )
	{
		return 6;
	}
	else
	{
		return -1;
	}

	return -2;
}

/* Update battery DATA in SIM808 handler */
void F0cks_SIM808_Battery_Update(SIM808_HandleTypeDef *handler)
{
	char *p = handler->privateStringBuffer;
	char tempo[5] = "";
	char *t = tempo;
	uint8_t i = 0;

	F0cks_SIM808_UART_Send("AT+CBC\n\r");

	while(F0cks_SIM808_Parse_String(handler) != 3);

	// Example:	+CBC: 0,52,3821
	p += 6;                                                 // Set on char '0'
	handler->battery.status = (*p++ - '0');                 // Store '0' as (int) 0 and go to ','
	while(*++p != ',')											                // Store in tempo '52'
		*t++ = *p;
	sscanf(tempo, "%d", (int *)&handler->battery.capacity);	// Store '52' as (int)
	for(i=0;i<5;i++)																				// Clean tempo buffer
		tempo[i] = '\0';
	t = tempo;																							// Reset pointer
	while(*++p != '\0')                                     // Store '3821' in tempo
		*t++ = *p;
	sscanf(tempo, "%d", (int *)&handler->battery.voltage);  // Store '3821' as (int)

	while(F0cks_SIM808_Parse_String(handler) != 1);

}

/* Enable GPRS */
void F0cks_SIM808_GPRS_Start(SIM808_HandleTypeDef *handler)
{
	int8_t value = 0;

	/* Network association */
	F0cks_SIM808_UART_Send("AT+CGATT=1\n\r");

	while(1)
	{
		value = F0cks_SIM808_Parse_String(handler);
		if(value == 6) // == ERROR
		{
			F0cks_SIM808_UART_Send("AT+CGATT=1\n\r");
			F0cks_Delay_ms(2000);
		}
		else if(value == 1) // == OK
		{
			break;
		}
	}

	/* Configure bearer profile 1 */
	F0cks_SIM808_UART_Send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK
	F0cks_SIM808_UART_Send("AT+SAPBR=3,1,\"APN\",\"");
	F0cks_SIM808_UART_Send(handler->apn);
	F0cks_SIM808_UART_Send("\"\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK

	/* Open GPRS context */
	F0cks_SIM808_UART_Send("AT+SAPBR=1,1\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK

	/* Initialize HTTP Service */
	F0cks_SIM808_UART_Send("AT+HTTPINIT\n\r");
	while( F0cks_SIM808_Parse_String(handler) != 1 ); // != OK
}

