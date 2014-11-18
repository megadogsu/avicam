#include <SD.h>
#define BUFF 63
File myFile;
unsigned char randNum, inByte, SOS;
const int chipSelect = 4;
int loopcount = 0;

// number of iterations to run
int numloops = 30000;
// 1000 iterations -> 2 minutes
// 30000 iterations -> 60 minutes

int errors = 0;
const int LEDpin = 8;

void setup(){
  	Serial.begin(57600);
  	randomSeed(473);
  	pinMode(13, OUTPUT);
  	// pin 10 must be output for SD
  	pinMode(10, OUTPUT);
  	pinMode(LEDpin, OUTPUT);

  	// if SD init fails, flash the LED
  	if (!SD.begin(4)) {
    	digitalWrite(LEDpin, HIGH);
    	delay(500);
    	digitalWrite(LEDpin, LOW);
    	delay(500);
    	digitalWrite(LEDpin, HIGH);
    	delay(500);
    	digitalWrite(LEDpin, LOW);
    	delay(500);
  	}

}

void loop(){
  	int i;
  	if((Serial.available() > 0) && (loopcount < numloops)){
    	// get start of session flag
    	SOS = Serial.read();

    	// start flag is wrong (also new)
    	if(SOS != 'S'){ 
      		digitalWrite(LEDpin, LOW);

      		myFile = SD.open("results.txt", FILE_WRITE);
      		if(myFile){
        		myFile.print("Incorrect start flag. Received: ");
        		myFile.print(SOS);
        		myFile.print(" at iteration: ");
        		myFile.println(loopcount);
        		myFile.close();
        		errors++;
      		}
    	}
    	// give acknowledge
    	Serial.write('A');

    	// wait for payload
    	for(i = 0; i < BUFF; i++){
    		while(Serial.available() < 1){}
    		inByte = Serial.read();
    		randNum = random(255);


    		// if we receive bad data
    		if(randNum != inByte){

      	  		// do SD stuff
      	  		myFile = SD.open("results.txt", FILE_WRITE);
      	  		if(myFile){
        			myFile.print("number generated (correct): ");
        			myFile.println(randNum);
        			myFile.print("number received (incorrect): ");
        			myFile.println(inByte);
        			myFile.print("at iteration: ");
        			myFile.println(loopcount);
        			myFile.print("\n");
        			myFile.close();
        			errors++;
      	  		}
      	  		digitalWrite(LEDpin, LOW);
    		}
    		// if no errors keep LED on
    		else if(loopcount < numloops){
    			if(errors > 0)
    				 digitalWrite(LEDpin, LOW);
    			else
    				 digitalWrite(LEDpin, HIGH);
    		}
    	} 
    	loopcount++;
  	}






  	if(loopcount == numloops){
    	myFile = SD.open("results.txt", FILE_WRITE);

    	if (myFile) {
      		myFile.print("\ntesting complete. total errors: ");
      		myFile.println(errors);
  			// close the file:
      		myFile.close();

    		// flash LED to finish test
    		for(int i = 0; i<5; i++){  
      			digitalWrite(LEDpin, HIGH);
      			delay(1000);
      			digitalWrite(LEDpin, LOW);
      			delay(1000);
      		}
    	}   

    	loopcount++; 
  	}


  	// end of loop()
}
