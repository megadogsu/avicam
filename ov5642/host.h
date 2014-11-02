#ifndef HOST_H
#define HOST_H

#include <Arduino.h>
#include <MemoryFree.h>
#include <SD.h>
//#include <SoftwareSerial.h>

class HostHelper{
	public:
		HostHelper(uint8_t RX = 2, uint8_t TX = 3);
		void checkMem();
		template<class T>void print(T);
		template<class T1, class T2>void print(T1,T2);
		template<class T>void println(T);
		template<class T1, class T2>void println(T1,T2);
		template<class T>void write(T);
		template<class T1, class T2>void write(T1,T2);
		template<class T>byte read(T);
		byte read();
		void begin(unsigned long);
		bool available();
		int fileCount();
		void listFiles(File dir, int numTabs = 1);
	private:
		//SoftwareSerial HostSerial;
		int FileCount;
};

template<class T>void HostHelper::print(T str){
	Serial.print(str);
}

template<class T>void HostHelper::println(T str){
	Serial.println(str);
}

template<class T>void HostHelper::write(T str){
	Serial.write(str);
}

template<class T>byte HostHelper::read(T str){
	return Serial.read(str);
}

template<class T1, class T2>void HostHelper::print(T1 str1, T2 str2){
	Serial.print(str1, str2);
}

template<class T1, class T2>void HostHelper::println(T1 str1, T2 str2){
	Serial.println(str1, str2);
}

template<class T1, class T2>void HostHelper::write(T1 str1, T2 str2){
	Serial.write(str1, str2);
}


#endif
