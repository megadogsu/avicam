#include "camera.h"

extern ArduCAM myCAM;
extern HostHelper Host;

byte CamControl::getByte(){
	while(!digitalRead(HREF_PIN)){}
	while(digitalRead(CLK_PIN)){}
	while(!digitalRead(CLK_PIN)){}
	return PINL;
	
	/*byte temp;
	while(true){
		while(!digitalRead(HREF_PIN)){}
		while(digitalRead(CLK_PIN)){}
		while(!digitalRead(CLK_PIN)){}
		return temp;
		temp = PINL;
		if(digitalRead(HREF_PIN))
			return temp;
		else
			continue;
		
	}*/
}

void CamControl::start()
{
	/*
  	uint8_t temp,temp_last,temp_first;
	unsigned long i = 0, j = 0;
    while( (temp != 0xD8) | (temp_first != 0xFF))
    {
    	temp_first = temp;
    	temp = getByte();
		//Host.print(temp, HEX);
    }
	
	FIFO[i++] = temp_first;
	FIFO[i++] = temp;

    while( (temp != 0xD9) | (temp_last != 0xFF) )
    {
    	temp_last = temp;
    	temp = getByte();
		FIFO[i++] = temp;
    }

	Host.println(i);
	
	for(j = 0; j < i; j++){
		Host.print(FIFO[j], HEX);
	}
	*/	
}

void CamControl::setup_capture()
{
    //Flush the FIFO 
    myCAM.flush_fifo();	
    //Clear the capture done flag 
    myCAM.clear_fifo_flag();		 
    //Start capture
    myCAM.start_capture();
}

void CamControl::saveToSD()
{
  	byte buf[SDBuffSize];
  	uint8_t temp,temp_last,temp_first;
  	unsigned long total_time = 0, i, j;

	//Host -> checkMem();
    //Construct a file name
    itoa(Host.fileCount()+1, str, 10); 
    strcat(str,".jpg");
    //Open the new file  
    outFile = SD.open(str,O_WRITE | O_CREAT | O_TRUNC);
    while (! outFile) 
    { 
      	Host.println(F("open file failed"));
      	delay(RetryDelay);
    }
    total_time = millis();
    //Read first dummy byte
    //myCAM.read_fifo();

    i = 0;
	j = 0;
    while( (temp != 0xD8) | (temp_first != 0xFF))
    {
    	temp_first = temp;
		while(Serial3.available() < 1){}
    	temp = Serial3.read();
		Host.print(temp, HEX);
    }
    //Write first image data to buffer
    buf[i++] = temp_first;
    buf[i++] = temp;
    Host.println(F("SOI Detected"));
	
    //Read JPEG data from FIFO
    while( (temp != 0xD9) | (temp_last != 0xFF) )
    {
      	temp_last = temp;
		while(Serial3.available() < 1){}
      	temp = Serial3.read();
      	//Host.print(temp,HEX);
		//Write image data to buffer if not full
      	if(i < SDBuffSize)
        	buf[i++] = temp;
      	else
      	{
        	//Write SDBuffSize bytes image data to file
        	while(outFile.write(buf,SDBuffSize) != SDBuffSize){
        		Host.println(F("Waiting SD I/O"));
        		delay(RetryDelay);
        		//Host -> checkMem();
				//Host.println(outFile.write(buf,SDBuffSize));
        	};
        	i = 0;
        	buf[i++] = temp;
      	}
    }
    //Write the remain bytes in the buffer
    if(i > 0){
    	while(outFile.write(buf,i) != i){
        	Host.println(F("Waiting SD I/O"));
        	delay(RetryDelay);
    	};
    }

	
    //Close the file 
    outFile.close(); 
    total_time = millis() - total_time;
    Host.print(F("Total time used:"));
    Host.print(total_time);
    Host.println(F(" millisecond"));    
    //Clear the capture done flag 
    myCAM.clear_fifo_flag();
    //Clear the start capture flag

    //myCAM.set_format(BMP);
    //myCAM.InitCAM();
    Host.println(F("Ready for another picture"));    
}

void CamControl::changeResolution(char str){
	if(str == 'H'){
		myCAM.wrSensorRegs16_8(OV5642_HD_setting);
		Host.println(F("Changed to HD(1920x1080) setting"));
	}else if(str == 'D'){
		myCAM.wrSensorRegs16_8(OV5642_DVGA_setting);
		Host.println(F("Changed to DVGA(960x540) setting"));
	}else if(str == 'V'){
		myCAM.wrSensorRegs16_8(OV5642_VGA_setting);
		Host.println(F("Changed to VGA(640x360) setting"));
	}else if(str == 'Q'){
		myCAM.wrSensorRegs16_8(OV5642_QVGA_setting);
		Host.println(F("Changed to QVGA(320x180) setting"));
	}else if(str == 'T'){
		myCAM.wrSensorRegs16_8(OV5642_Thumbnail_setting);
		Host.println(F("Changed to Thumbnail(160x90) setting"));
	}else{
		Host.println(F("No such setting"));
	}
   	myCAM.flush_fifo();
 	delay(50);
}

