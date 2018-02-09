#ifndef F0CKS_SIM808_H
#define F0CKS_SIM808_H

#include "stdint.h"

#define STRING_BUFFER_SIZE 100

typedef struct
{
	uint8_t *  uartCircularBuffer;
	uint8_t uartCircularBufferSize;

}SIM808_ConfigurationTypeDef;

/**
  * @brief  SIM808 handle Structure definition
  */
typedef struct
{
	uint8_t *  uartCircularBuffer;
	uint8_t uartCircularBufferSize;

	uint8_t *  privateCircularBufferP;
	uint8_t    privateStringBuffer[STRING_BUFFER_SIZE];
	int8_t  ack;

}SIM808_HandleTypeDef;

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config);
void F0cks_SIM808_Power_ON(SIM808_HandleTypeDef *handler);
void F0cks_SIM808_Power_OFF(void);
int8_t F0cks_Read_Circular_Buffer(SIM808_HandleTypeDef *handler);
int8_t F0cks_SIM808_Compare_Strings(char *str1, char *str2);

/* Functions to implement */
void F0cks_Delay_ms(uint32_t ms);
void F0cks_SIM808_PWRKEY_High(void);
void F0cks_SIM808_PWRKEY_Low(void);
void F0cks_SIM808_UART_Send(char *string);

#endif //F0CKS_SIM808_H
