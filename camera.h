#ifndef CAMERA_H
#define CAMERA_H

#include <avr/pgmspace.h>
#include <UTFT_SPI.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include "host.h"

#if defined(__arm__)
#include <itoa.h>
#endif

#define IRIS_PIN 5
// set pin 10 as the slave select for SPI:

const int SPI_CS = 10;

const struct sensor_reg OV5642_HD_setting[] PROGMEM = {
	{0x3808 ,0x07},
	{0x3809 ,0x80},
	{0x380a ,0x04},
	{0x380b ,0x38},

	{0xffff, 0xff},
};

const struct sensor_reg OV5642_DVGA_setting[] PROGMEM = {
	{0x3808 ,0x03},
	{0x3809 ,0xC0},
	{0x380a ,0x02},
	{0x380b ,0x1C},

	{0xffff, 0xff},
};

const struct sensor_reg OV5642_VGA_setting[] PROGMEM = {
	{0x3808 ,0x02},
	{0x3809 ,0x80},
	{0x380a ,0x01},
	{0x380b ,0x68},

	{0xffff, 0xff},
};

const struct sensor_reg OV5642_QVGA_setting[] PROGMEM = {
	{0x3808 ,0x01},
	{0x3809 ,0x40},
	{0x380a ,0x00},
	{0x380b ,0xB4},

	{0xffff, 0xff},
};


const struct sensor_reg OV5642_Thumbnail_setting[] PROGMEM = {
	{0x3808 ,0x00},
	{0x3809 ,0xA0},
	{0x380a ,0x00},
	{0x380b ,0x5A},

	{0xffff, 0xff},
};

class CamControl{
	public:
		void start();
		void setup_capture();
		void saveToSD();
		void changeResolution(char);
	private:
        char str[8];
		static const unsigned int SDBuffSize = 128;
		static const short RetryDelay = 20;
    	File outFile;
};


#endif //CAMERA_H
