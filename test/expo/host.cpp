#include "host.h"

HostHelper::HostHelper(uint8_t RX, uint8_t TX)
	//Serial1(RX, TX)
{
}

void HostHelper::begin(unsigned long baud){
	//UCSR0A |= (1 << U2X0); 
	
	Serial1.begin(baud);
	UBRR1L = 17;
	Serial1.setTimeout(10000);
}

void HostHelper::checkMem(){
	Serial1.print("Free Memory:");
	Serial1.println(freeMemory());
}


void HostHelper::listFiles(char* dir,int numTabs){
	File root = SD.open(dir);
	root.seek(0);
	while(true) {
     	File entry =  root.openNextFile();
     	if (! entry) {
       		// no more files
			root.close();
       		break;
     	}
     	for (uint8_t i=0; i<numTabs; i++) {
       		print('\t');
     	}
     	print(entry.name());
     	if (entry.isDirectory()) {
       		println("/");
			listFiles(entry.name(), numTabs+1);
     	} else {
       		// files have sizes, directories do not
       		print("\t\t");
       		println(entry.size(), DEC);
     	}
     	entry.close();
   	}
}

void HostHelper::eraseFiles(char* dir){
	File root = SD.open(dir);
	root.seek(0);
	while(true) {
     	File entry =  root.openNextFile();
     	if (! entry) {
       		// no more files
			root.close();
       		break;
     	}
		SD.remove(entry.name());		
		entry.close();
   	}
}

byte HostHelper::read(){
	return Serial1.read();
}

byte HostHelper::readBytesUntil(char terminal, char* buffer, unsigned int length){
	Serial1.setTimeout(0);
	return Serial1.readBytesUntil(terminal, buffer, length);
	Serial1.setTimeout(10000);
}

void HostHelper::readUntil(char terminal, char* buffer, unsigned int length){
	int len = Serial1.readBytesUntil(terminal, buffer, length);
	buffer[len] = '\0';
}


bool HostHelper::available(){
	return Serial1.available();
}

int HostHelper::fileCount(char *dir){
	int FileCount = 0;
	File root = SD.open(dir);
	root.seek(0);
	while(true) {
     	File entry =  root.openNextFile();
     	if (! entry) {
			root.close();
       		return FileCount;
       		break;
     	}
		entry.close();
		FileCount++;
	}
}


