#include "xbee.h"


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
