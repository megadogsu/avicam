#include <SdFat.h>
#include <SdFatUtil.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

SdFat sd;
SdFile myFile;

const uint8_t chipSelect = 4;


#define SDBuffSize 50
#define XBeeBuffSize 256

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  Serial.begin(19200);
  mySerial.begin(19200);
  Serial.setTimeout(10000);
  mySerial.setTimeout(10000);
  pinMode(10, OUTPUT);  
  
  while(Serial.available() < 0){}
  delay(400);
  
  if (sd.begin(chipSelect, SPI_HALF_SPEED)){
    mySerial.println("initialization done.");
  }else{
    mySerial.println("initialization failed!");
    return;
  }
}

void SerialInt(int n){
  mySerial.write(lowByte(n));
  mySerial.write(highByte(n));
}


void loop()
{	
  	if (mySerial.available())
  	{ // If data comes in from serial monitor, send it out to XBee
    	char key; 
    	char terminal = '\n';
    	char resend = 'R';
    	int i, fileSize, remain, error;

		while(mySerial.available() < 1){}
		key = mySerial.read();
    	switch(key){
      			
      		case 'G':
        		char XBeeBuff[XBeeBuffSize+1], echo;
        		if(myFile.open("tmp.jpg", O_READ)){
          			fileSize = myFile.fileSize();
          			SerialInt(fileSize);
          			mySerial.println(fileSize);
          			i = 0;
		  			while(i < fileSize){
		  	  			i += myFile.read(XBeeBuff, XBeeBuffSize);
          	  			do{
		  	  	  			if(i == fileSize){
		  	  	  	  			XBeeBuff[fileSize%XBeeBuffSize] = terminal;
		  	  	  	  			mySerial.write((uint8_t*)XBeeBuff,XBeeBuffSize+1);
				  			}
		  	  	  			else{
		  	  	  	  			XBeeBuff[XBeeBuffSize] = terminal;
		  	  	  	  			mySerial.write((uint8_t*)XBeeBuff,XBeeBuffSize+1);
		  	  	  			}
		  	  	  			while(mySerial.available() < 1){}
			  	  			echo = mySerial.read();
          	  			}while(echo != terminal);
		  			}
          			// close the file:
          			myFile.close();
        		}
        		break;
      		case 'T':
        		char high, low;
        		char SDBuff[SDBuffSize+1];

        		Serial.write('C');
        		if(sd.exists("tmp.jpg")){
        			while(!sd.remove("tmp.jpg")){
        				mySerial.println("Fault removing files");
        				delay(100);
        			}
        		}
        		if(myFile.open("tmp.jpg", O_CREAT | O_WRITE)){
          			Serial.write('#');
          			while(Serial.available() < 2){}
          			high = Serial.read();
          			low = Serial.read();
          			fileSize = makeWord(high, low);

          			i = 0; 
          			error = 0;

          			while(i < fileSize){
			  			if(i > (fileSize - SDBuffSize)){
          	  	  			remain = fileSize%SDBuffSize;
			  	  			while(Serial.available() < remain+1){}
          	  	  			Serial.readBytes(SDBuff,remain+1);
          	  	  			if(SDBuff[remain] != terminal){
              	  	  			Serial.write(resend);
              	  	  			error++;
              	  	  			continue;
              	  			}else{
              	  	  			Serial.write(terminal);
              	  	  			int tmp = myFile.write(SDBuff,remain);
              	  	  			mySerial.println(tmp);
              	  	  			while(tmp < remain){
              	  	  				tmp = myFile.write(SDBuff,remain);
              	  	  			}
              	  	  			//while(myFile.write(SDBuff,remain) < remain){}
              	  	  			i += remain;
              	  	  			break;
              	  			}
          	  			}else{
          	  	  			while(Serial.available() < SDBuffSize+1){}
              	  			Serial.readBytes(SDBuff,SDBuffSize+1);
              	  			if(SDBuff[SDBuffSize] != terminal){
              	  	  			Serial.write(resend);
              	  	  			error++;
              	  			}else{
              	  	  			Serial.write(terminal);
              	  	  			int tmp = myFile.write(SDBuff,SDBuffSize);
              	  	  			mySerial.println(tmp);
              	  	  			while(tmp < SDBuffSize){
              	  	  				tmp = myFile.write(SDBuff,SDBuffSize);
              	  	  				mySerial.println(tmp);
									mySerial.println(FreeRam());
          	  						myFile.sync();
              	  	  				delay(100);
              	  	  			}
              	  	  			/*while(myFile.write(SDBuff,SDBuffSize) < SDBuffSize){
              	  	  			  delay(10);
              	  	  			  }*/
              	  	  			i += SDBuffSize;
              	  			}

          	  			}
          	  			myFile.sync();
          			}
          			mySerial.print("Successfully saved to SD!, size:");
          			mySerial.println(myFile.fileSize());
          			Serial.print("Transfer Done! with errors:");
          			Serial.println(error);
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

