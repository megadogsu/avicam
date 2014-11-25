ARDUINO_DIR = /usr/share/arduino
ARDUINO_PORT  = /dev/ttyACM0
BOARD_TAG    = uno
MONITOR_PORT  = /dev/ttyUSB0
ARDUINO_LIBS = Wire/utility SPI Wire SD SD/utility SoftwareSerial UTFT_SPI ArduCAM MemoryFree
ARDMK_DIR = /usr/local
ARDMK_PATH = /usr/share/arduino
MONITOR_BAUDRATE = 57600

include /usr/share/arduino/Arduino.mk
usb:
	make upload \
	ARDUINO_PORT=/dev/ttyACM0 \
	AVRDUDE_ARD_BAUDRATE=115200
xbee:
	make upload \
	ARDUINO_PORT=/dev/ttyUSB0 \
	AVRDUDE_ARD_BAUDRATE=115200

