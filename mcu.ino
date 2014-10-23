#include <SdFat.h>
#include <SdFatUtil.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

SdFat sd;
SdFile myFile;

const uint8_t chipSelect = 4;


#define SDBuffSize 50
#define XBeeBuffSize 256
#define RetryDelay 20

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  Serial.begin(57600);
  mySerial.begin(19200);
  Serial.setTimeout(10000);
  mySerial.setTimeout(10000);
  pinMode(10, OUTPUT);  
  
  while(mySerial.available() < 0){}
  //delay(400);
  
  if (sd.begin(chipSelect, SPI_HALF_SPEED)){
    mySerial.println("initialization done.");
  }else{
    mySerial.println("initialization failed!");
    return;
  }
}

void SerialInt(unsigned long n){
  mySerial.write(lowByte(n & 0xFFFF));
  mySerial.write(highByte(n & 0xFFFF));
  mySerial.write(lowByte(n >> 16));
  mySerial.write(highByte(n >> 16));
}


void loop()
{	
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
      		case 'T':
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
          			fileSize = (makeWord(sizebytes[0], sizebytes[1]) << 8) + makeWord(sizebytes[2], sizebytes[3]);
          			Serial.println(fileSize);
          			
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

      		case 'Z':
        		mySerial.print("Sending image size:");
        		if(myFile.open("tmp.jpg", O_READ)){        
          			// read from the file until there's nothing else in it:
          			mySerial.println(myFile.fileSize());
          			// close the file:
          			myFile.close();
        		}else{
        			mySerial.println("Failed to open img.jpg");
        		}
        		break;

      		case 'R':
  	  	  	  	if (!myFile.open("test.txt", O_READ)) {
    				sd.errorHalt("opening test.txt for read failed");
  	  	  	  	}
  	  	  	  	mySerial.println("test.txt:");

  	  	  	  	// read from the file until there's nothing else in it:
  	  	  	  	int data;
  	  	  	  	while ((data = myFile.read()) >= 0) mySerial.write(data);
  	  	  	  	// close the file:
  	  	  	  	mySerial.println("Done!");
  	  	  	  	myFile.close();
				break;
  	  	  	case 'W':
        		if(myFile.open("test.txt", O_CREAT | O_WRITE)){        
          			// read from the file until there's nothing else in it:
          			myFile.println("Test,1,2,3...");
          			// close the file:
          			myFile.close();
        			mySerial.println("Write test file successfully!");
        		}else{
        			mySerial.println("Failed to open test.txt");
        		}
        		break;
      		default:
        		//Serial.write(key);
        		break;
    	}
  	}
}

