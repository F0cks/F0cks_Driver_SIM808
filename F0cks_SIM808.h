#ifndef F0CKS_SIM808_H
#define F0CKS_SIM808_H

#include "stdint.h"

#define STRING_BUFFER_SIZE 100
#define PIN_SIZE             6
#define APN_SIZE            15

typedef struct
{
  uint16_t  errors;
  uint16_t  requestOver;

}SIM808_HttpTypeDef;

typedef struct
{
  uint8_t  status;
  uint16_t capacity;
  uint16_t voltage;

}SIM808_BatteryTypeDef;

typedef struct
{
	uint8_t *uartCircularBuffer;
	uint8_t  uartCircularBufferSize;

	char *apn;
	char *pinCode;

}SIM808_ConfigurationTypeDef;

/**
  * @brief  SIM808 handle Structure definition
  */
typedef struct
{
	uint8_t *uartCircularBuffer;
	uint8_t  uartCircularBufferSize;
	char apn[APN_SIZE];
	char pinCode[PIN_SIZE];

	uint8_t *privateCircularBufferP;
	char     privateStringBuffer[STRING_BUFFER_SIZE];
	uint8_t  ackValue;

	SIM808_BatteryTypeDef battery;

	SIM808_HttpTypeDef		http;

}SIM808_HandleTypeDef;

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config);
void   F0cks_SIM808_Power_ON(SIM808_HandleTypeDef *handler);
void   F0cks_SIM808_Power_OFF(SIM808_HandleTypeDef *handler);
void   F0cks_SIM808_GSM_Start(SIM808_HandleTypeDef *handler);
void   F0cks_SIM808_Battery_Update(SIM808_HandleTypeDef *handler);
void   F0cks_SIM808_GPRS_Start(SIM808_HandleTypeDef *handler);
int8_t F0cks_SIM808_send_SMS(SIM808_HandleTypeDef *handler, char *number, char *message);
void   F0cks_SIM808_GPS_Start(SIM808_HandleTypeDef *handler);
void   F0cks_SIM808_GPS_Stop(SIM808_HandleTypeDef *handler);
void   F0cks_SIM808_HTTP_Head_Request(SIM808_HandleTypeDef *handler, char *url);

/* Functions to implement */
void F0cks_Delay_ms(uint32_t ms);
void F0cks_SIM808_PWRKEY_High(void);
void F0cks_SIM808_PWRKEY_Low(void);
void F0cks_SIM808_UART_Send(char *string);

#endif //F0CKS_SIM808_H
