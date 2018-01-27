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

#endif //F0CKS_SIM808_H
