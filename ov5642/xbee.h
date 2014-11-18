#ifndef XBEE_H
#define XBEE_H

#include <Arduino.h>
#include <ArduCAM.h>
#include <SD.h>
#include "host.h"

class XBeeData{
	public:
		void transfer();	
		void video();	
	private:
		void SerialInt(unsigned long);

    	static const char terminal = '\n';
		static const unsigned int XBeeBuffSize = 512;
		static const short RetryDelay = 20;
		char str[8];
    	int readbytes;
        char XBeeBuff[XBeeBuffSize+1], echo;
        File myFile;
};

#endif //XBEE_H
