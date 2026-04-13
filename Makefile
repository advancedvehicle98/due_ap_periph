# надо platformio через venv включить!!!!!!!!!!!

ENV = release

ifeq ($(DEBUG),1)
ENV = debug
endif

UAVCAN_MODULES = \
	protocol.GetNodeInfo_res \
	protocol.dynamic_node_id.Allocation

UAVCAN_SRC = $(foreach M,$(UAVCAN_MODULES),./dsdl/src/uavcan.$(M).c)

all: dsdl compile

dsdl:
	./modules/dronecan_dsdlc/dronecan_dsdlc.py -O ./dsdl ./modules/DSDL/uavcan/ ./modules/DSDL/ardupilot/ ./modules/DSDL/dronecan/
	mkdir -p ./include/dronecan
	mkdir -p ./src/dronecan
	cp ./dsdl/include/* ./include/dronecan/
	cp $(UAVCAN_SRC) ./src/dronecan/
	cp ./modules/libcanard/canard.h ./include/
	cp ./modules/libcanard/canard_internals.h ./include/
	cp ./modules/libcanard/canard.c ./src

compile:
	pio run -e $(ENV)

upload:
	pio run -e $(ENV) -t upload

clean:
	rm -rf ./dsdl ./include/dronecan ./src/dronecan ./include/canard.h ./src/canard.c ./include/canard_internals.h
