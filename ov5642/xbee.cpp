#include "xbee.h"

extern ArduCAM myCAM;
extern HostHelper Host;

void XBeeData::SerialInt(unsigned long n){
  	Host.write(lowByte(n & 0xFFFF));
  	Host.write(highByte(n & 0xFFFF));
  	Host.write(lowByte(n >> 16));
  	Host.write(highByte(n >> 16));
}

void XBeeData::transfer()
{

	unsigned long i, fileSize;
    itoa(Host.fileCount() , str, 10); 
    strcat(str,".jpg");
    //Host.println(str);
    myFile = SD.open(str, O_READ);
    if(myFile){
        fileSize = myFile.size();
        //Host.println(fileSize);
        Host.checkMem();
        SerialInt(fileSize);
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
		  	  		Host.write((uint8_t*)XBeeBuff,fileSize%XBeeBuffSize);
		  	  		Host.write(terminal);
				}
		  	  	else{
		  	  		Host.write((uint8_t*)XBeeBuff,XBeeBuffSize);
		  	  		Host.write(terminal);
		  	  	}
		  	  	while(Host.available() < 1){}
			  	echo = Host.read();
          	}while(echo != terminal);
		}
        // close the file:
        Host.print(F("Transfer Done! Total:"));
        Host.println(myFile.size());
        myFile.close();
    }else{
        Host.println(F("Fault opening files"));
    }
}


void XBeeData::video(){
	uint8_t temp_first = 0x00, temp = 0x00, temp_last = 0x00;
	int i;
    i = 0;
    while( (temp != 0xD8) | (temp_first != 0xFF))
    {
    	temp_first = temp;
    	temp = myCAM.read_fifo();
		//Host.print(temp,HEX);
    }
    //Write first image data to buffer
    XBeeBuff[i++] = temp_first;
    XBeeBuff[i++] = temp;
    //Host.println(F("SOI Detected"));

    //Read JPEG data from FIFO
    while( (temp != 0xD9) | (temp_last != 0xFF) )
    {
      	temp_last = temp;
      	temp = myCAM.read_fifo();
      	//Host.print(temp,HEX);

		//Write image data to buffer if not full
      	if(i < XBeeBuffSize)
        	XBeeBuff[i++] = temp;
      	else
      	{
        	//Write SDBuffSize bytes image data to file
		  	//Host.write((uint8_t*)XBeeBuff,XBeeBuffSize);
			do{
		  	   	Host.write((uint8_t*)XBeeBuff,XBeeBuffSize);
		  	   	Host.write(terminal);
		  	   	while(Host.available() < 1){}
			   	echo = Host.read();
        	}while(echo != terminal);

        	i = 0;
        	XBeeBuff[i++] = temp;
      	}
    }
    //Write the remain bytes in the buffer
    if(i > 0){
		//Host.write((uint8_t*)XBeeBuff,i);
        do{
		  	Host.write((uint8_t*)XBeeBuff,i);
		  	Host.write(terminal);
		  	while(Host.available() < 1){}
		  	echo = Host.read();
        }while(echo != terminal);
    }
	//Host.print("Finished");
    //Clear the capture done flag 
    myCAM.clear_fifo_flag();
}
