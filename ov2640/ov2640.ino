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

ArduCAM myCAM(OV2640,SPI_CS);
UTFT myGLCD(SPI_CS);
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

  	myGLCD.InitLCD();

  	//Check if the camera module type is OV2640
  	myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  	myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  	if((vid != 0x26) || (pid != 0x42))
  		mySerial.println("Can't find OV2640 module!");
  	else
  		mySerial.println("OV2640 detected");

  	//Change to BMP capture mode and initialize the OV2640 module	  	
  	myCAM.set_format(BMP);

  	myCAM.InitCAM();

  	while(mySerial.available() < 0){}

  	//Initialize SD Card
  	if (!SD.begin(SD_CS)) 
  	{
    	//while (1);		//If failed, stop here
    	mySerial.println("initialization failed!");
    	mySerial.println("SD Card Error");
  	}
  	else{
    	mySerial.println("SD Card detected!");
    	mySerial.println("initialization done.");
    }
  	//delay(400);

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
        		/*
        		char XBeeBuff[XBeeBuffSize+1], echo;
        		if(myFile.open("tmp.jpg", O_READ)){
          		   	fileSize = myFile.fileSize();
          		   	SerialInt(fileSize);
          			//mySerial.println(fileSize);
          			i = 0;
		  			while(i < fileSize){
						readbytes = myFile.read(XBeeBuff, XBeeBuffSize);
		  	  			while(readbytes < 0){
							readbytes = myFile.read(XBeeBuff, XBeeBuffSize);
							delay(RetryDelay);
		  	  			}

		  	  			i += readbytes;

          	  			do{
		  	  				if(i == fileSize){
		  	  					mySerial.write((uint8_t*)XBeeBuff,fileSize%XBeeBuffSize);
		  	  					mySerial.write(terminal);
							}
		  	  				else{
		  	  					mySerial.write((uint8_t*)XBeeBuff,XBeeBuffSize);
		  	  					mySerial.write(terminal);
		  	  				}
		  	  				while(mySerial.available() < 1){}
			  				echo = mySerial.read();
          	  			}while(echo != terminal);
		  			}
          			// close the file:
          			mySerial.print("Transfer Done! Total:");
          			mySerial.println(myFile.fileSize());
          			myFile.close();
        		}else{
        			mySerial.println("Fault opening files");
        		}
        		break;
				*/
      		case 'T':
				mySerial.println("Capture Signal Received");
				/*
        		char high, low;
        		char SDBuff[SDBuffSize+1];

        		Serial.write('C');
        		if(sd.exists("tmp.jpg")){
        			while(!sd.remove("tmp.jpg")){
        				mySerial.println("Fault removing files");
        				delay(RetryDelay);
        			}
        		}
        		if(myFile.open("tmp.jpg", O_CREAT | O_RDWR)){
          			Serial.write('#');
          			while(Serial.available() < 4){}
          			Serial.readBytes(sizebytes,4);
          			fileSize = ((unsigned long)makeWord(sizebytes[3], sizebytes[2]) << 16) + makeWord(sizebytes[1], sizebytes[0]);
          			Serial.println(fileSize);
          			Serial.write((uint8_t*)sizebytes,4);

          			i = 0; 
          			error = 0;
          			sync = 0;

          			while(i < fileSize){
			  			if(i > (fileSize - SDBuffSize)){
          	  	  			remain = fileSize%SDBuffSize;
			  	  			while(Serial.available() < remain){
			  					//mySerial.println("123");	
			  					delay(RetryDelay);	
			  				}
          	  				Serial.readBytes(SDBuff,remain);
			  				while(Serial.available() < 1){
			  					//mySerial.println("234");	
			  					delay(RetryDelay);	
			  				}
          	  				if(Serial.read() != terminal){
              					Serial.write(resend);
              					error++;
              					continue;
              				}else{
              					Serial.write(terminal);
              					int tmp = myFile.write(SDBuff,remain);
              					while(tmp < remain){
          	  						myFile.sync();
        							delay(RetryDelay);
              						tmp = myFile.write(SDBuff,remain);
              						sync++;
              						//mySerial.println("File I/O");
              					}
              					i += remain;
              					break;
              				}
          	  			}else{
          	  				while(Serial.available() < SDBuffSize){
			  					//mySerial.println(Serial.available());
			  					delay(RetryDelay);	
          	  				}
              				Serial.readBytes(SDBuff,SDBuffSize);
			  				while(Serial.available() < 1){
			  					//mySerial.println("456");	
			  					//mySerial.println(Serial.available());
			  					delay(RetryDelay);	
			  				}
              				if(Serial.read() != terminal){
              					Serial.write(resend);
              					error++;
              				}else{
              					Serial.write(terminal);
              					int tmp = myFile.write(SDBuff,SDBuffSize);
              					while(tmp < SDBuffSize){
          	  						myFile.sync();
        							delay(RetryDelay);
              						tmp = myFile.write(SDBuff,SDBuffSize);
              						sync++;
              						//mySerial.println("File I/O");
        						}
        						i += SDBuffSize;
        					}

        				}

        			}
        			mySerial.print("Successfully saved to SD!, size:");
        			mySerial.println(myFile.fileSize());
        			Serial.print("Transfer Done! with packet errors:");
        			Serial.print(error);
        			Serial.print(" and SD I/Os:");
        			Serial.println(sync);
        			myFile.sync();
        			myFile.close();
        		}else{
          			Serial.write('!');
        		}
        		myFile.close();
        		break;
				*/
    			isShowFlag = false;
    			myCAM.write_reg(ARDUCHIP_MODE, 0x00);
    			myCAM.set_format(JPEG);
    			myCAM.InitCAM();

    			//myCAM.OV2640_set_JPEG_size(OV2640_640x480);
    			myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
    			//Wait until buttom released
    			while(myCAM.read_reg(ARDUCHIP_TRIG) & SHUTTER_MASK){}
    			delay(1000);
    			start_capture = 1;
				break;
    		default:
    			break;
  		}
	}
    if(isShowFlag )
    {
    	temp = myCAM.read_reg(ARDUCHIP_TRIG);

    	if(!(temp & VSYNC_MASK))				 			//New Frame is coming
    	{
    		myCAM.write_reg(ARDUCHIP_MODE, 0x00);    		//Switch to MCU
    		myGLCD.resetXY();
    		myCAM.write_reg(ARDUCHIP_MODE, 0x01);    		//Switch to CAM
    		while(!(myCAM.read_reg(ARDUCHIP_TRIG)&0x01)){} 	//Wait for VSYNC is gone
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
  	}
}

void myloop()
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

	//Wait trigger from shutter buttom   
	if(myCAM.read_reg(ARDUCHIP_TRIG) & SHUTTER_MASK)	
	{
		isShowFlag = false;
		myCAM.write_reg(ARDUCHIP_MODE, 0x00);
		myCAM.set_format(JPEG);
		myCAM.InitCAM();

		//myCAM.OV2640_set_JPEG_size(OV2640_640x480);
		myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
		//Wait until buttom released
		while(myCAM.read_reg(ARDUCHIP_TRIG) & SHUTTER_MASK);
		delay(1000);
		start_capture = 1;

	}
	else
	{
		if(isShowFlag )
		{
			temp = myCAM.read_reg(ARDUCHIP_TRIG);

			if(!(temp & VSYNC_MASK))				 			//New Frame is coming
			{
				myCAM.write_reg(ARDUCHIP_MODE, 0x00);    		//Switch to MCU
				myGLCD.resetXY();
				myCAM.write_reg(ARDUCHIP_MODE, 0x01);    		//Switch to CAM
				while(!(myCAM.read_reg(ARDUCHIP_TRIG)&0x01)); 	//Wait for VSYNC is gone
			}
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
		Serial.println("Start Capture");     
	}

	if(myCAM.read_reg(ARDUCHIP_TRIG) & CAP_DONE_MASK)
	{

		Serial.println("Capture Done!");

		//Construct a file name
		k = k + 1;
		itoa(k, str, 10); 
		strcat(str,".jpg");
		//Open the new file  
		outFile = SD.open(str,O_WRITE | O_CREAT | O_TRUNC);
		if (! outFile) 
		{ 
			Serial.println("open file failed");
			return;
		}
		total_time = millis();
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
    	Serial.print("Total time used:");
    	Serial.print(total_time, DEC);
    	Serial.println(" millisecond");    
    	//Clear the capture done flag 
    	myCAM.clear_fifo_flag();
    	//Clear the start capture flag
    	start_capture = 0;

    	myCAM.set_format(BMP);
    	myCAM.InitCAM();
    	isShowFlag = true;	
	}
}




