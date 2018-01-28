#include "F0cks_SIM808.h"

int8_t F0cks_SIM808_Init( SIM808_HandleTypeDef *handler, SIM808_ConfigurationTypeDef config)
{
	int8_t error = 0;

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
