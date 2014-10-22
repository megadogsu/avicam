#include <SdFat.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX


SdFat sd;
SdFile myFile, theFile;

const uint8_t chipSelect = SS;


#define buffsize 62

void setup()
{
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  Serial.begin(19200);
  mySerial.begin(19200);
  Serial.setTimeout(10000);
  mySerial.setTimeout(10000);
  //pinMode(10, OUTPUT);  
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
    char key = mySerial.read();
    char terminal = '\n';
    switch(key){
      case 'G':
        if(myFile.open("img.jpg", O_READ)){
          SerialInt(myFile.fileSize());
          mySerial.println(myFile.fileSize());
          // read from the file until there's nothing else in it:
          while (myFile.available()) {
          	mySerial.write(myFile.read());
          }
          // close the file:
          myFile.close();
        }
        break;
      case 'T':
        char high, low;
        int i, bytes, remain, error;
        char data[buffsize+1];
        
        Serial.write('C');
        sd.remove("img.jpg");
        if(myFile.open("img.jpg", O_CREAT | O_WRITE)){
          Serial.write('#');
          while(Serial.available() < 2){}
          high = Serial.read();
          low = Serial.read();
          bytes = makeWord(high, low);
          
          i = 0; 
          error = 0;

          while(i < bytes){
			  if(i > (bytes - buffsize)){
          	  	  remain = bytes%buffsize;
			  	  while(Serial.available() < remain+1){}
          	  	  Serial.readBytes(data,remain+1);
          	  	  if(data[remain] != terminal){
              	  	  Serial.write('R');
              	  	  error++;
              	  	  continue;
              	  }else{
              	  	  Serial.write(terminal);
              	  	  myFile.write(data,remain);
              	  	  i += remain;
              	  	  break;
              	  }
          	  }else{
          	  	  while(Serial.available() < buffsize+1){}
              	  Serial.readBytes(data,buffsize+1);
              	  if(data[buffsize] != terminal){
              	  	  Serial.write('R');
              	  	  error++;
              	  }else{
              	  	  Serial.write(terminal);
              	  	  myFile.write(data,buffsize);
              	  	  i += buffsize;
              	  }

          	  }
          }

          /*
          for(i = 0; i < bytes; i+=buffsize){
              //Serial.println(i);

              if( i > bytes - buffsize){
          	  	  remain = bytes%buffsize;
				  while(Serial.available() < remain){}
          	  	  Serial.readBytes(data,remain+1);
          	  	  if(data[remain] != terminal){
              	  	  Serial.write('R');
              	  	  i-=buffsize;
              	  	  error++;
              	  	  continue;

              	  	  Serial.write(terminal);
              	  	  myFile.write(data,remain);
              	  	  break;
            	  }
              }
			  while(Serial.available() < buffsize+1){
			  	  mySerial.println(Serial.available());
			  }
              Serial.readBytes(data,buffsize+1);
              if(data[buffsize] != terminal){
              	  Serial.write('R');
              	  i-=buffsize;
              	  error++;
              }else{
              	  Serial.write(terminal);
              	  myFile.write(data,buffsize);
              }
          }
          */
          mySerial.println("Successfully saved to SD!");
          Serial.print("Transfer Done! with errors:");
          Serial.println(error);
          myFile.close();
          //Serial.println(myFile.fileSize());
        }else{
          	Serial.write('!');
        }
        break;
      case 'Z':
        mySerial.print("Sending image size:");
        if(theFile.open("img.jpg", O_READ)){        
          	// read from the file until there's nothing else in it:
          	mySerial.println(theFile.fileSize());
          	// close the file:
          	theFile.close();
        }
        break;
      default:
        //Serial.write(key);
        break;
    }
  }
}

