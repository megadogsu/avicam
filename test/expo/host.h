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
		byte readBytesUntil(char terminal, char* buffer, unsigned int length = 16);
		void readUntil(char terminal, char* buffer, unsigned int length = 16);
		void begin(unsigned long);
		bool available();
		int fileCount(char* dir = "/");
		void listFiles(char* dir = "/", int numTabs = 0);
		void eraseFiles(char* dir = "/");
	private:


};

template<class T>void HostHelper::print(T str){
	Serial1.print(str);
}

template<class T>void HostHelper::println(T str){
	Serial1.println(str);
}

template<class T>void HostHelper::write(T str){
	Serial1.write(str);
}

template<class T>byte HostHelper::read(T str){
	return Serial1.read(str);
}

template<class T1, class T2>void HostHelper::print(T1 str1, T2 str2){
	Serial1.print(str1, str2);
}

template<class T1, class T2>void HostHelper::println(T1 str1, T2 str2){
	Serial1.println(str1, str2);
}

template<class T1, class T2>void HostHelper::write(T1 str1, T2 str2){
	Serial1.write(str1, str2);
}


#endif
