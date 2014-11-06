#include "host.h"

HostHelper::HostHelper(uint8_t RX, uint8_t TX)
	//Serial(RX, TX)
{
}

void HostHelper::begin(unsigned long baud){
	Serial.begin(baud);
	Serial.setTimeout(10000);
}

void HostHelper::checkMem(){
	Serial.print("Free Memory:");
	Serial.println(freeMemory());
}


void HostHelper::listFiles(File dir,int numTabs){
	//File root = SD.open("/");
	dir.seek(0);
	while(true) {
     	File entry =  dir.openNextFile();
     	if (! entry) {
       		// no more files
			dir.close();
       		break;
     	}
     	for (uint8_t i=0; i<numTabs; i++) {
       		print('\t');
     	}
     	print(entry.name());
     	if (entry.isDirectory()) {
       		println("/");
			listFiles(entry, numTabs+1);
     	} else {
       		// files have sizes, directories do not
       		print("\t\t");
       		println(entry.size(), DEC);
     	}
     	entry.close();
   	}
}


byte HostHelper::read(){
	return Serial.read();
}

byte HostHelper::readBytesUntil(char terminal, char* buffer, unsigned int length){
	return Serial.readBytesUntil(terminal, buffer, length);
}


bool HostHelper::available(){
	return Serial.available();
}

int HostHelper::fileCount(File dir){
	int FileCount = 0;
	//File root = SD.open("/");
	dir.seek(0);
	while(true) {
     	File entry =  dir.openNextFile();
     	if (! entry) {
			dir.close();
       		return FileCount;
       		break;
     	}
		entry.close();
		FileCount++;
	}
}


