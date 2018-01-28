#ifndef F0CKS_SIM808_H
#define F0CKS_SIM808_H

#include "stdint.h"

typedef struct
{

}SIM808_ConfigurationTypeDef;

/**
  * @brief  SIM808 handle Structure definition
  */
typedef struct
{

}SIM808_HandleTypeDef;

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config);
void F0cks_SIM808_Power_ON(void);
void F0cks_SIM808_Power_OFF(void);

/* Functions to implement */
void F0cks_Delay_ms(uint32_t ms);
void F0cks_SIM808_PWRKEY_High(void);
void F0cks_SIM808_PWRKEY_Low(void);

#endif //F0CKS_SIM808_H
