#include "gps.h"

extern HostHelper Host;

GPSDevice::GPSDevice(uint8_t RX, uint8_t TX)
	:gpsSerial(RX,TX)
{	
}

void GPSDevice::begin(unsigned long baud){
	gpsSerial.begin(9600);
}

void GPSDevice::GetData()
{
	GPSData gpsData;
	gpsData.GetGPS(gpsSerial);	
}


void GPSData::GetGPS(SoftwareSerial& gpsSerial)
{
  	static int i = 0;
  	int GPSSentenceLength = 0, ReadComplete = 0;
	//  gpsSerial.listen();
  	while (ReadComplete == 0)
  	{
    	if (gpsSerial.available())
    	{
      		char ch = gpsSerial.read();
      		if (ch != '\n' && i < GPSSentenceSize)
      		{
        		GPSSentence[i] = ch;
        		i++;
      		}
      		else
      		{
       			GPSSentence[i] = '\0';
       			GPSSentenceLength = i;
       			i = 0;
       			if ((GPSSentence[0]=='$')&&(GPSSentence[4]=='G'))
       			{
          			// SaveGPSData(GPSSentenceLength);
          			ReadComplete = 1;
					//          dummy.listen();
					//Host.println(GPSSentence);
					displayGPS();
       			}
      		}
    	}
  	}
}

void GPSData::SaveGPSData(int GPSSentenceLength)
{
  	int DataField = 0, SentencePos = 0, FieldPos = 0;
  	while ( (SentencePos < GPSSentenceLength) && (DataField < 10) ) {
    	if (GPSSentence[SentencePos] == ',')
    	{
      		gpsData[DataField][FieldPos] = '\0';
      		FieldPos = 0;
      		DataField++;
    	} else
    	{
      		gpsData[DataField][FieldPos] = GPSSentence[SentencePos];
      		FieldPos ++;
    	}
    	SentencePos ++;
  	}
  	gpsData[DataField][FieldPos] = '\0';
}

void GPSData::displayGPS()
{
    Host.print(F("Time: "));
    Host.println(gpsData[1]);
    Host.print(F("Lat: "));
    Host.print(gpsData[2]);
    Host.println(gpsData[3]);
    Host.print(F("Long: "));
    Host.print(gpsData[4]);
    Host.println(gpsData[5]);
    Host.print(F("GPS quality: "));
    Host.println(gpsData[6]);
    Host.print(F("Altitude :"));
    Host.println(gpsData[9]);
}

