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

#include <UTFT_SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

#if defined(__arm__)
#include <itoa.h>
#endif

#define SD_CS 9 
// set pin 10 as the slave select for SPI:
const int SPI_CS = 10;

ArduCAM myCAM(OV5642,SPI_CS);

boolean isShowFlag = true;

void SerialInt(unsigned long n){
  	mySerial.write(lowByte(n & 0xFFFF));
  	mySerial.write(highByte(n & 0xFFFF));
  	mySerial.write(lowByte(n >> 16));
  	mySerial.write(highByte(n >> 16));
}

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
  	mySerial.begin(19200);
  	while(mySerial.available() < 0){}
  	mySerial.println("XBee Link OK");
  	mySerial.println("ArduCAM Start!"); 
  	// set the SPI_CS as an output:
  	pinMode(SPI_CS, OUTPUT);

  	// initialize SPI:
  	SPI.begin(); 
  	//Check if the ArduCAM SPI bus is OK
  	myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  	temp = myCAM.read_reg(ARDUCHIP_TEST1);
  	if(temp != 0x55)
  	{
  	  	mySerial.println("SPI interface Error!");
  	  	while(1);
  	}

  	//Change MCU mode
  	myCAM.write_reg(ARDUCHIP_MODE, 0x00);


  	//Check if the camera module type is OV5642
  	myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  	myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  	if((vid != 0x56) || (pid != 0x42))
  	  	mySerial.println("Can't find OV5642 module!");
  	else
  	  	mySerial.println("OV5642 detected");

  	//Change to BMP capture mode and initialize the OV5642 module	  	
  	myCAM.set_format(BMP);

  	myCAM.InitCAM();

  	//Initialize SD Card
  	if (!SD.begin(SD_CS)) 
  	{
      	//while (1);		//If failed, stop here
      	mySerial.println("SD Card Error");
  	}
  	else{
      	mySerial.println("SD Card detected!");
  	  	mySerial.println("initialization done.");
  	}
}

void loop()
{	
  	char str[8];
  	File outFile;
  	byte buf[256];
  	static int i = 0;
  	static int k = 0;
  	static int n = 0;
  	uint8_t temp,temp_last;
  	uint8_t start_capture = 0;
  	int total_time = 0;
  	if (mySerial.available())
  	{ // If data comes in from serial monitor, send it out to XBee
    	char key; 
    	char terminal = '\n';
    	char resend = 'R';
    	int remain, error, sync, readbytes;
		char sizebytes[4];
		unsigned long i, fileSize;
		while(mySerial.available() < 1){}
		key = mySerial.read();
    	switch(key){

      		case 'G':
        		break;
      		case 'T':
				mySerial.println("Capture Signal Received");
    			isShowFlag = false;
    			myCAM.write_reg(ARDUCHIP_MODE, 0x00);
    			myCAM.set_format(JPEG);
  				myCAM.InitCAM();
  				myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);		//VSYNC is active HIGH

    			//Wait until buttom released
    			start_capture = 1;
				break;
    		default:
    			break;
  		}
	}
  	if(start_capture)
  	{
    	//Flush the FIFO 
    	myCAM.flush_fifo();	
    	//Clear the capture done flag 
    	myCAM.clear_fifo_flag();		 
    	//Start capture
    	myCAM.start_capture();
    	mySerial.println("Start Capture");     
  	}

  	if(myCAM.read_reg(ARDUCHIP_TRIG) & CAP_DONE_MASK)
  	{

    	mySerial.println("Capture Done!");

    	//Construct a file name
    	k = k + 1;
    	itoa(k, str, 10); 
    	strcat(str,".jpg");
    	//Open the new file  
    	outFile = SD.open(str,O_WRITE | O_CREAT | O_TRUNC);
    	if (! outFile) 
    	{ 
      		mySerial.println("open file failed");
      		return;
    	}
    	total_time = millis();
    	//Read first dummy byte
    	//myCAM.read_fifo();

    	i = 0;
    	temp = myCAM.read_fifo();
    	//Write first image data to buffer
    	buf[i++] = temp;

    	//Read JPEG data from FIFO
    	while( (temp != 0xD9) | (temp_last != 0xFF) )
    	{
      		temp_last = temp;
      		temp = myCAM.read_fifo();
      		//Write image data to buffer if not full
      		if(i < 256)
        		buf[i++] = temp;
      		else
      		{
        		//Write 256 bytes image data to file
        		outFile.write(buf,256);
        		i = 0;
        		buf[i++] = temp;
      		}
    	}
    	//Write the remain bytes in the buffer
    	if(i > 0)
      		outFile.write(buf,i);

    	//Close the file 
    	outFile.close(); 
    	total_time = millis() - total_time;
    	mySerial.print("Total time used:");
    	mySerial.print(total_time, DEC);
    	mySerial.println(" millisecond");    
    	//Clear the capture done flag 
    	myCAM.clear_fifo_flag();
    	//Clear the start capture flag
    	start_capture = 0;

    	myCAM.set_format(BMP);
    	myCAM.InitCAM();
    	isShowFlag = true;	
    	mySerial.println("Ready for another picture");    
  	}
}

