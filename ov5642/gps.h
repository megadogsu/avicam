#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "host.h"

class GPSData;

class GPSDevice{
	public:
		GPSDevice(uint8_t RX = 8, uint8_t TX = 9);
		void begin(unsigned long);
		void GetData();
	private:	
		SoftwareSerial gpsSerial;
};

class GPSData{
	public:
		void GetGPS(SoftwareSerial&);
	private:
		void SaveGPSData(int);
		void displayGPS();	
		static const int GPSSentenceSize = 80;
		char GPSSentence[GPSSentenceSize];
		char gpsData[10][11];
};

#endif //GPS_H

