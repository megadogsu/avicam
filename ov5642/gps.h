#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "host.h"

class GPSData{
	public:
		GPSData(uint8_t RX = 8, uint8_t TX = 9);
		void GetGPS();
	private:
		void SaveGPSData(int);
		void displayGPS();	
		static const int GPSSentenceSize = 80;
		char GPSSentence[GPSSentenceSize];
		char gpsData[10][11];
		SoftwareSerial gpsSerial;
};

#endif //GPS_H

