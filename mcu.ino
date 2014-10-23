#include <SdFat.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX


SdFat sd;
SdFile myFile;

const uint8_t chipSelect = SS;


#define SDBuffSize 62
#define XBeeBuffSize 512

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
        sd.remove("tmp.jpg");
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
              	  	  	while(myFile.write(SDBuff,remain) < 0){};
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
              	  	  	while(myFile.write(SDBuff,SDBuffSize) <  0){};
              	  	  	i += SDBuffSize;
              	  	}

          	  	}
          	}
          	mySerial.print("Successfully saved to SD!, size:");
          	mySerial.println(myFile.fileSize());
          	Serial.print("Transfer Done! with errors:");
          	Serial.println(error);
          	myFile.close();
        	/*
        	myFile.open("img.jpg", O_READ);
        	mySerial.println(myFile.fileSize());
          	myFile.close();
			*/
          	//Serial.println(myFile.fileSize());
        }else{
          	Serial.write('!');
        }
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


      case 'W':
        if(myFile.open("test.txt", O_CREAT | O_WRITE)){        
          	// read from the file until there's nothing else in it:
          	myFile.write("Test, 1, 2, 3 ...");
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

