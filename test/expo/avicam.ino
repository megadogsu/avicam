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
#include "xbee.h"
#include "camera.h"
#include "host.h"

#define SD_CS 9 


ArduCAM myCAM(OV2640,SPI_CS);
CamControl *cam;
XBeeData *xbeeData;
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
  	Host.begin(115200);
  	
  	// set the SPI_CS as an output:
  	pinMode(SPI_CS, OUTPUT);
  	pinMode(IRIS_PIN, OUTPUT);

  	// initialize SPI:
  	SPI.begin(); 
  	//Check if the ArduCAM SPI bus is OK

  	//Change MCU mode
  	
  	myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  	myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);

  	if((vid != 0x26) || (pid != 0x42))
  		Host.println("Can't find OV2640 module!");
  	else
  		Host.println("OV2640 detected");

  	myCAM.set_format(JPEG);

  	myCAM.InitCAM();
  	myCAM.wrSensorReg8_8(0x11, 0x3F);
    myCAM.OV2640_set_JPEG_size(OV2640_160x120);
  	
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
	pinMode(CLK_PIN,INPUT);
	DDRL = 0;
}

void loop()
{
	if(Host.available()){
  		// If data comes in from serial monitor, send it out to XBee
		char key;
		
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
				cam = new CamControl();
				cam->start();
				Host.checkMem();
				cam->saveToSD();
				delete cam;
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
      		case 'L':
				Host.listFiles();
      			break;
      		case 'M':
				Host.checkMem();
      			break;
      		case 'R':
    			{
    				char str[16];
					Host.readUntil('\n', str, 16);   
					if(SD.remove(str))
						Host.println(F("Successful removed"));   		
					else
						Host.println(F("Failed removing"));   		
					Host.checkMem();
      				break;
      			}
      		case 'E':
				Host.eraseFiles();
				break;
      		case 'S':
				{
    				char str;
					Host.println(F("Available Setting: HD(H), QVGA(Q), Thumbnail(T)"));
					while(Host.available() < 1){}
					str = Host.read();
					cam = new CamControl();
					cam->changeResolution(str);
					delete cam;
					break;
				}
      		case 'V':
      			{
					char stop;
					Host.println(F("Starting Video"));
					cam = new CamControl();
					//cam->changeResolution("Thumb");
      				xbeeData = new XBeeData();
					do{
						cam->start();
      					xbeeData -> video();
						if(Host.available() > 0){
							stop = Host.read();
							Host.println(stop);
						}else{
							stop = '\0';
						}
      				}while(stop != 'Q');
      				Host.print('\n');
      				delete xbeeData;
					delete cam;
					break;
				}
      		case 'X':
				break;
    		default:
    			break;
  		}
  	}
}

