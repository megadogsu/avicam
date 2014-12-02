#ifndef XBEE_H
#define XBEE_H

#include <Arduino.h>
#include <ArduCAM.h>
#include <SD.h>
#include "host.h"

#define XBEE

class XBeeData{
	public:
		void transfer();	
		void video();	
	private:
		void SerialInt(unsigned long);

    	static const char terminal = '\n';
		static const unsigned int XBeeBuffSize = 256;
		static const unsigned int DIBuffSize = 1024;
		static const short RetryDelay = 20;

#ifndef XBEE
		static const short RFDelay = 480; //XBee Pro
		static const short PacketDelay = 160; //XBee Pro

#else

		static const short RFDelay = 36; //XBee
		static const short PacketDelay = 64; //XBee Pro
#endif
		char str[8];
    	int readbytes;
        char XBeeBuff[XBeeBuffSize+1], echo;
        File myFile;
};

#endif //XBEE_H
