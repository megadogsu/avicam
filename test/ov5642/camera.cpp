#include "camera.h"

extern ArduCAM myCAM;
extern HostHelper Host;

void CamControl::start()
{
    myCAM.write_reg(ARDUCHIP_MODE, 0x00);
  	myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);		//VSYNC is active HIGH
    //Wait until buttom released
    setup_capture();
    //Host.println(F("Start Capture"));     
  	while(myCAM.read_reg(ARDUCHIP_TRIG) & CAP_DONE_MASK){}
    //Host.println(F("Sensor Capture Done!"));
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
  	int total_time = 0, i;

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
    while( (temp != 0xD8) | (temp_first != 0xFF))
    {
    	temp_first = temp;
    	temp = myCAM.read_fifo();
		//Host.print(temp,HEX);
    }
    //Write first image data to buffer
    buf[i++] = temp_first;
    buf[i++] = temp;
    Host.println(F("SOI Detected"));
	
    //Read JPEG data from FIFO
    while( (temp != 0xD9) | (temp_last != 0xFF) )
    {
      	temp_last = temp;
      	temp = myCAM.read_fifo();
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

	/*	
	i = 0;
	unsigned thumbbytes = 0;
    Host.println(F("Started Thumbnail"));
    while( (temp != 0xD8) | (temp_first != 0xFF))
    {
    	temp_first = temp;
    	temp = myCAM.read_fifo();
		thumbbytes++;
		Host.print(temp,HEX);
    }
    
	buf[i++] = temp_first;
    buf[i++] = temp;
	Host.print('\n');
    Host.println(F("SOI Detected"));
    Host.println(thumbbytes);
    
	while( (temp != 0xD9) | (temp_last != 0xFF) )
    {
      	temp_last = temp;
      	temp = myCAM.read_fifo();
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
	*/	

	
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

void CamControl::changeResolution(char* str){
	String setting = String(str);
	/*	
	Host.checkMem();
	prog_char hd[] PROGMEM = "HD";
	prog_char qvga[] PROGMEM = "QVGA";
	prog_char thumbnail[] PROGMEM = "Thumbnail";
	Host.checkMem();
	
	PROGMEM const char *resolution[] = {   
  		hd,
  		qvga,
  		thumbnail
  	};
	Host.checkMem();

	char buffer[16];
	strcpy_P(buffer, (char*)pgm_read_word((resolution[0])));
	String HD = String(buffer);
	strcpy_P(buffer, (char*)pgm_read_word((resolution[1])));
	String QVGA = String(buffer);
	strcpy_P(buffer, (char*)pgm_read_word((resolution[2])));
	String Thumbnail = String(buffer);

	Host.checkMem();
	Host.print(HD);
	Host.print(Thumbnail);
	Host.checkMem();
	*/	
	if(setting == "H" || setting == "HD"){
		myCAM.wrSensorRegs16_8(OV5642_HD_setting);
		Host.println(F("Changed to HD(1920x1080) setting"));
	}else if(setting == "Q" || setting == "QVGA"){
		myCAM.wrSensorRegs16_8(OV5642_QVGA_setting);
		Host.println(F("Changed to QVGA(320x180) setting"));
	}else if(setting == "T" || setting == "Thumb"){
		myCAM.wrSensorRegs16_8(OV5642_Thumbnail_setting);
		Host.println(F("Changed to Thumbnail(160x90) setting"));
	}else{
		Host.println(F("No such setting"));
	}
   	myCAM.flush_fifo();
 	delay(50);
}

