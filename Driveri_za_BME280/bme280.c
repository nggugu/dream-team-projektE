/*
 * bme280.c
 *
 *  Created on: Dec 23, 2021
 *      Author: gugu
 */

#include "bme280.h"
#include "i2c.h"

/*
 * Weather monitoring configuration
 */
BME280_config weatherMonitoring = {
		.config_reg = 0x00,		// t_sb unimportant, IIR filter off, SPI disabled
		.ctrl_hum = 0x01,		// humidity oversampling = x1
		.ctrl_meas = 0x26		// temperature oversampling = x1, pressure oversampling = x1, forced mode
};

/*
 * Humidity sensing configuration
 */
BME280_config humiditySensing = {
		.config_reg = 0x00,		// t_sb unimportant, IIR filter off, SPI disabled
		.ctrl_hum = 0x01,		// humidity oversampling = x1
		.ctrl_meas = 0x22		// temperature oversampling = x1, pressure skipped, forced mode
};

/*
 * Indoor navigation configuration
 */
BME280_config indoorNavigation = {
		.config_reg = 0x10,		// t_sb = 0.5 ms, IIR coeficient = 16, SPI disabled
		.ctrl_hum = 0x01,		// humidity oversampling = x1
		.ctrl_meas = 0x97		// temperature oversampling = x8, pressure oversampling = x16, normal mode
};

/*
 * Gaming configuration
 */
BME280_config gaming = {
		.config_reg = 0x10,		// t_sb = 0.5 ms, IIR coeficient = 16, SPI disabled
		.ctrl_hum = 0x00,		// humidity skipped
		.ctrl_meas = 0x2F		// temperature oversampling = x1, pressure oversampling = x4, normal mode
};

/*
 * Everything set to max
 */
BME280_config iber = {
		.config_reg = 0x10,		// t_sb = 0.5 ms, IIR coeficient = 16, SPI disabled
		.ctrl_hum = 0x07,		// humidity oversampling = x16
		.ctrl_meas = 0xFF		// temperature oversampling = x16, pressure oversampling = x16, normal mode
};

/*
 * Extract calibration data from sensor
 */
void readCalibrationData(void) {
	uint8_t CallibData_TP[24] = {0};
	uint8_t CallibData_H[7] = {0};

	// Calibration data for temperature and pressure
	HAL_I2C_Mem_Read(&i2c_channel, BME280_ADDRESS_READ, 0x88, 1, CallibData_TP, 24, HAL_MAX_DELAY);

	dig_T1 = ((CallibData_TP[1] << 8) | CallibData_TP[0]);
	dig_T2 = ((CallibData_TP[3] << 8) | CallibData_TP[2]);
	dig_T3 = ((CallibData_TP[5] << 8) | CallibData_TP[4]);

	dig_P1 = ((CallibData_TP[7] << 8) | CallibData_TP[6]);
	dig_P2 = ((CallibData_TP[9] << 8) | CallibData_TP[8]);
	dig_P3 = ((CallibData_TP[11] << 8) | CallibData_TP[10]);
	dig_P4 = ((CallibData_TP[13] << 8) | CallibData_TP[12]);
	dig_P5 = ((CallibData_TP[15] << 8) | CallibData_TP[14]);
	dig_P6 = ((CallibData_TP[17] << 8) | CallibData_TP[16]);
	dig_P7 = ((CallibData_TP[19] << 8) | CallibData_TP[18]);
	dig_P8 = ((CallibData_TP[21] << 8) | CallibData_TP[20]);
	dig_P9 = ((CallibData_TP[23] << 8) | CallibData_TP[22]);

	// Calibration data for humidity
	HAL_I2C_Mem_Read(&i2c_channel, BME280_ADDRESS_READ, 0xA1, 1, CallibData_H, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&i2c_channel, BME280_ADDRESS_READ, 0xE1, 1, CallibData_H + 1, 6, HAL_MAX_DELAY);

	dig_H1 = CallibData_H[0];
	dig_H2 = ((CallibData_H[2] << 8) | CallibData_H[1]);
	dig_H3 = CallibData_H[3];
	dig_H4 = ((CallibData_H[4] << 4) | (CallibData_H[5] & 0x0F));
	dig_H5 = (((CallibData_H[5] & 0xF0) >> 4) | (CallibData_H[7] << 4));
}

/*
 * Initialize BME280 sensor according to given configuration
 */
void initBME280(BME280_config config, I2C_HandleTypeDef i2c_address) {
	uint8_t bytes[2] = {0};
	i2c_channel = i2c_address;

	// Reset BME280
	bytes[0] = 0xE0;
	bytes[1] = 0xB6;
	HAL_I2C_Master_Transmit(&i2c_channel, BME280_ADDRESS_WRITE, bytes, 2, HAL_MAX_DELAY);

	// Set configuration register
	bytes[0] = 0xF5;
	bytes[1] = config.config_reg;
	HAL_I2C_Master_Transmit(&i2c_channel, BME280_ADDRESS_WRITE, bytes, 2, HAL_MAX_DELAY);

	// Set oversampling rates and device mode
	bytes[0] = 0xF2;
	bytes[1] = config.ctrl_hum;
	HAL_I2C_Master_Transmit(&i2c_channel, BME280_ADDRESS_WRITE, bytes, 2, HAL_MAX_DELAY);

	bytes[0] = 0xF4;
	bytes[1] = config.ctrl_meas;
	HAL_I2C_Master_Transmit(&i2c_channel, BME280_ADDRESS_WRITE, bytes, 2, HAL_MAX_DELAY);

	readCalibrationData();
}

/*
 * Set device to forced mode
 */
void performMeasureBME280(void) {
	uint8_t bytes[2] = {0};

	// Get the ctrl_meas settings
	bytes[0] = 0xF4;
	HAL_I2C_Master_Transmit(&i2c_channel, BME280_ADDRESS_WRITE, bytes, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&i2c_channel, BME280_ADDRESS_READ, bytes+1, 1, HAL_MAX_DELAY);

	// Set device to forced mode
	bytes[1] = bytes[1] | 0x01;
	HAL_I2C_Master_Transmit(&i2c_channel, BME280_ADDRESS_WRITE, bytes, 2, HAL_MAX_DELAY);
}

void readData() {
	uint8_t uData[8] = {0};

	HAL_I2C_Mem_Read(&i2c_channel, BME280_ADDRESS_READ, 0xF7, 1, uData, 8, HAL_MAX_DELAY);

	uncompPres = ((uData[0] << 12) | (uData[1] << 4) | (uData[2] >> 4));
	uncompTemp = ((uData[3] << 12) | (uData[4] << 4) | (uData[5] >> 4));
	uncompHum = ((uData[6] << 8) | uData[7]);

	getTemp(uncompTemp);
	getPres(uncompPres);
	getHum(uncompHum);
}

/*
 * Get calibrated temperature
 */
void getTemp(uint32_t uTemp) {

	double var1, var2, T;

	var1 = (((double) uTemp) / 16384.0 - ((double) dig_T1) / 1024.0) * ((double) dig_T2);
	var2 = ((((double) uTemp) / 131072.0 - ((double) dig_T1) / 8192.0) *
			(((double) uTemp) / 131072.0 - ((double) dig_T1) / 8192.0)) * ((double) dig_T3);

	t_fine = (int32_t) (var1 + var2);
	T = (var1 + var2) / 5120.0;

	temperature = T;
}

/*
 * Get calibrated pressure
 */
void getPres(uint32_t uPres) {

	double var1, var2, p;

	var1 = ((double) t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double) dig_P5) * 2.0;
	var2 = (var2 / 4.0)+(((double) dig_P4) * 65536.0);
	var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0)*((double) dig_P1);

	if (var1 == 0.0) {
		p = 0;
	}

	p = 1048576.0 - (double) uPres;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double) dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double) dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double) dig_P7)) / 16.0;

	pressure = p;
}

/*
 * Get calibrated humidity
 */
void getHum(uint16_t uHum) {

	double var_H;

	var_H = (((double) t_fine) - 76800.0);
	var_H = (uHum - (((double) dig_H4) * 64.0 + ((double) dig_H5) / 16384.0 *
			var_H)) * (((double) dig_H2) / 65536.0 * (1.0 + ((double) dig_H6) /
					67108864.0 * var_H *
					(1.0 + ((double) dig_H3) / 67108864.0 * var_H)));
	var_H = var_H * (1.0 - ((double) dig_H1) * var_H / 524288.0);

	if (var_H > 100.0) {
		var_H = 100.0;
	} else if (var_H < 0.0) {
		var_H = 0.0;
	}

	humidity = var_H;
}
