# надо platformio через venv включить!!!!!!!!!!!

ENV = release

ifeq ($(DEBUG),1)
ENV = debug
endif

all: dsdl compile

dsdl:
	./modules/dronecan_dsdlc/dronecan_dsdlc.py -O ./dsdl ./modules/DSDL/uavcan/ ./modules/DSDL/ardupilot/ ./modules/DSDL/dronecan/
	mkdir -p ./include/dronecan
	mv ./dsdl/include/* ./include/dronecan/
	cp ./modules/libcanard/canard.h ./include/

compile:
	pio run -e $(ENV)

upload:
	pio run -e $(ENV) -t upload

clean:
	rm -rf ./dsdl ./include/dronecan ./include/canard.h
