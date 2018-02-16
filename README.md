# F0cks_Driver_SIM808

## How to use this library

1. You need to create files like **sim.c** and **sim.h**.
2. In those files, you need to implement several functions:
	* **void F0cks_Delay_ms(uint32_t ms);**
	* **void F0cks_SIM808_PWRKEY_High(void);**
	* **void F0cks_SIM808_PWRKEY_Low(void);**
	* **void F0cks_SIM808_UART_Send(char *string);**
3. Create a handler : **SIM808_HandleTypeDef hsim;**.
4. Prepare configuration, for example:**
SIM808_ConfigurationTypeDef sim808Config = {
		.uartCircularBuffer     = UART1_Rx_Buffer,
		.uartCircularBufferSize = 200,
		.pinCode                = "1234",
		.apn                    = "free"
};**.
5. Initialize the handler with: **F0cks_SIM808_Init(&hsim, sim808Config);**.
6. Start the module with : **F0cks_SIM808_Power_ON(&hsim);**.
7. Stop the module with : **F0cks_SIM808_Power_OFF(&hsim);**.

### How to use GSM

1. Start GSM with : **F0cks_SIM808_GSM_Start(&hsim);**

### How to use GPRS

1. You need to start GSM first
2. Start GPRS communication with : **F0cks_SIM808_GPRS_Start(&hsim);**
3. You can send HTTP request (head method) with : **F0cks_SIM808_HTTP_Head_Request(&hsim, url);**
4. You will not be able to send an other request until hsim.http.requestOver is equal to 1
5. Error counter (hsim.http.errors) is incremented each time an error occured 

### How to use GPS

1. Start GPS with : **F0cks_SIM808_GPS_Start(&hsim);**
2. Update GPS values (stored in hsim.gps) with : **F0cks_SIM808_GPS_Update(&hsim);**
(By default utcDateTime latitude longitude altitude speed are equal to '\0')
3. hsim.gps.newData will be set to 1 when GPS store correct values
4. You need to set **hsim.gps.newData=0;** to use **F0cks_SIM808_GPS_Update(&hsim);** again
5. You can stop GPS with : **F0cks_SIM808_GPS_Stop(&hsim);**

## Release description

* v0.0.3 : 16/02/2018
	* Create functions to start/stop/update GPS values
	* Create a function to send HTTP request with HEAD method

* v0.0.2 : 13/02/2018
	* Reworking word parser: prevent overflow, parse all words in one function
	* Create a function to store battery DATA
	* Create a function to start GMS
	* Create a function to start GPRS
	* Create a function to send SMS

* v0.0.1 : 09/02/2018
	* Create functions needed start and stop SIM808 module