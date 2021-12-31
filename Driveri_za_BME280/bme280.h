/*
 * bme280.h
 *
 *  Created on: Dec 23, 2021
 *      Author: gugu
 *
 *      API for BME280 sensor using I2C communication.
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_

#include "main.h"

// Device addresses
#define BME280_ADDRESS_WRITE	0xEC
#define BME280_ADDRESS_READ		0xED

// Structure for device configuration
typedef struct BME280_config {
	uint8_t config_reg;
	uint8_t ctrl_hum;
	uint8_t ctrl_meas;
} BME280_config;

/*
 * Weather monitoring configuration
 */
extern BME280_config weatherMonitoring;

/*
 * Humidity sensing configuration
 */
extern BME280_config humiditySensing;

/*
 * Indoor navigation configuration
 */
extern BME280_config indoorNavigation;

/*
 * Gaming configuration
 */
extern BME280_config gaming;

/*
 * Everything set to max
 */
extern BME280_config iber;

// This variable holds the desired I2C channel
I2C_HandleTypeDef i2c_channel;

// Variables to hold calibration data
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t dig_H6;

// Variables to hold uncompensated data
uint32_t uncompTemp;
uint32_t uncompPres;
uint16_t uncompHum;

// This variable carries fine temperature as global value
int32_t t_fine;

// Variables to hold calibrated data
extern double temperature;
extern double pressure;
extern double humidity;

/*
 * This function is called once upon a start of a device.
 * It extracts the calibration data from the sensor.
 */
void readCalibrationData(void);

/*
 * Initialize BME280 sensor according to given configuration
 */
void initBME280(BME280_config config, I2C_HandleTypeDef i2c_address);

/*
 * Use this function if your sensor works in weather mode
 */
void performMeasureBME280(void);

/*
 * Read and calibrate data from sensor
 */
void readData(void);

// This set of functions are used to get raw data from the sensor.

/*
 * Calculates temperature in DegC, double precision.
 * Output value of “51.23” equals 51.23 DegC.
 */
void getTemp(uint32_t uTemp);

/*
 * Calculates pressure in Pa as double.
 * Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
 */
void getPres(uint32_t uPres);

/*
 * Calculates humidity in %rH as as double.
 * Output value of “46.332” represents 46.332 %rH
 */
void getHum(uint16_t uHum);

#endif /* INC_BME280_H_ */
