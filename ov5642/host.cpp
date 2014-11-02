#include "host.h"

HostHelper::HostHelper(uint8_t RX, uint8_t TX)
	//Serial(RX, TX)
{
}

void HostHelper::begin(unsigned long baud){
	Serial.begin(baud);
}

void HostHelper::checkMem(){
	Serial.print("Free Memory:");
	Serial.println(freeMemory());
}


void HostHelper::listFiles(File dir, int numTabs){
	while(true) {
     	File entry =  dir.openNextFile();
     	if (! entry) {
       		// no more files
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
	dir.close();
}


byte HostHelper::read(){
	return Serial.read();
}

bool HostHelper::available(){
	return Serial.available();
}

int HostHelper::fileCount(File dir){
	int FileCount = 0;   
	while(true) {
     	File entry =  dir.openNextFile();
     	if (! entry) {
       		return FileCount;
       		break;
     	}
		FileCount++;
	}
	dir.close();
}


