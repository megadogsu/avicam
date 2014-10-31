#include "host.h"

HostHelper::HostHelper(uint8_t RX, uint8_t TX):
	HostSerial(RX, TX)
{
}

void HostHelper::begin(unsigned long baud){
	HostSerial.begin(baud);
}

void HostHelper::checkMem(){
	HostSerial.print("Free Memory:");
	HostSerial.println(freeMemory());
}


byte HostHelper::read(){
	return HostSerial.read();
}

bool HostHelper::available(){
	return HostSerial.available();
}

int HostHelper::fileCount(){
	return FileCount;
}


