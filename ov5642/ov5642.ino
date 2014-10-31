// ArduCAM demo (C)2014 Lee
// web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with a supported camera modules, and can run on any Arduino platform.
//
// This demo was made for Omnivision OV2640 2MP sensor.
// It will run the ArduCAM as a real 2MP digital camera, provide both preview and JPEG capture.
// The demo sketch will do the following tasks:
// 1. Set the sensor to BMP preview output mode.
// 2. Switch to JPEG mode when shutter buttom pressed.
// 3. Capture and buffer the image to FIFO. 
// 4. Store the image to Micro SD/TF card with JPEG format.
// 5. Resolution can be changed by myCAM.set_JPEG_size() function.
// This program requires the ArduCAM V3.1.0 (or later) library and Rev.C ArduCAM shield
// and use Arduino IDE 1.5.2 compiler or above

#include <SD.h>
#include "camera.h"
#include "xbee.h"
#include "host.h"

#define SD_CS 9 

ArduCAM myCAM(OV5642,SPI_CS);

HostHelper Host;

void setup()
{
  	uint8_t vid,pid;
  	uint8_t temp; 
#if defined (__AVR__)
    Wire.begin(); 
#endif
#if defined(__arm__)
    Wire1.begin(); 
#endif
  	Serial.begin(115200);
  	Host.begin(57600);
	
  	while(Serial.available() < 0){}
  	while(Host.available() < 0){}
	
	Host.checkMem();
  		
  	Serial.println(F("Serial Link OK"));
  	
  	Host.println(F("XBee Link OK"));
  	// set the SPI_CS as an output:
  	pinMode(SPI_CS, OUTPUT);
  	pinMode(IRIS_PIN, OUTPUT);

  	// initialize SPI:
  	SPI.begin(); 
  	//Check if the ArduCAM SPI bus is OK
  	myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  	temp = myCAM.read_reg(ARDUCHIP_TEST1);
  	if(temp != 0x55)
  	{
  	  	Host.println(F("SPI interface Error!"));
  	  	while(1);
  	}

  	//Change MCU mode
  	myCAM.write_reg(ARDUCHIP_MODE, 0x00);


  	//Check if the camera module type is OV5642
  	myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  	myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  	if((vid != 0x56) || (pid != 0x42))
  	  	Host.println(F("Can't find OV5642 module!"));
  	else
  	  	Host.println(F("OV5642 detected"));

  	//Change to BMP capture mode and initialize the OV5642 module	  	
  	//myCAM.set_format(BMP);
  	//myCAM.InitCAM();
    //myCAM.set_format(JPEG);
	//myCAM.wrSensorRegs16_8(OV5642_5M_Pixel_setting);
	byte reg_val;
	myCAM.wrSensorReg16_8(0x3008, 0x80);

	delay(100);
	//myCAM.wrSensorRegs16_8(OV5642_1080P_Video_setting);
	myCAM.wrSensorRegs16_8(OV5642_5M_Pixel_setting);
	myCAM.rdSensorReg16_8(0x3818,&reg_val);
	myCAM.wrSensorReg16_8(0x3818, (reg_val | 0x20) & 0xBf);
	myCAM.rdSensorReg16_8(0x3621,&reg_val);
	myCAM.wrSensorReg16_8(0x3621, reg_val | 0x20);

  	//myCAM.InitCAM();
    myCAM.flush_fifo();

  	//Initialize SD Card
  	if (!SD.begin(SD_CS)) 
  	{
      	//while (1);		//If failed, stop here
      	Host.println(F("SD Card Error"));
  	}
  	else{
      	Host.println(F("SD Card detected!"));
  	  	Host.println(F("initialization done."));
		Host.checkMem();
  	}
}

void loop()
{
	
  	if (Host.available())
  	{ // If data comes in from serial monitor, send it out to XBee
    	char key; 
		Capture *capture = NULL;
		XBeeData *xbeeData = NULL;

		while(Host.available() < 1){}
		key = Host.read();
    	switch(key){

      		case 'G':
      			xbeeData = new XBeeData();
      			xbeeData -> transfer();
      			delete xbeeData;
      			break;
      		case 'T':
				Host.println(F("Capture Signal Received"));
				capture = new Capture();
				capture->start();
				Host.checkMem();
				capture->save_SD();
				delete capture;
				Host.checkMem();
				break;
      		case 'O':
				Host.println(F("Opening Iris"));
      			digitalWrite(IRIS_PIN,HIGH);
        		break;
      		case 'C':
				Host.println(F("Closing Iris"));
      			digitalWrite(IRIS_PIN,LOW);
      			break;
    		default:
    			Host.write(key);
    			break;
  		}
	}
}

