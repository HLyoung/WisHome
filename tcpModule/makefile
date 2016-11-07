SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I ./
INCLUDE_PATH+= -I ./include/
INCLUDE_PATH+= -I ../include/

LIB_PATH=-L ./
LIB_PATH+=-L ../lib

LIB=-lmyLog

DEFINE=DEBUG
OBJ=../lib/libtcpModule.so
OBJ:$(SRC)
	@echo "make tcpmodule"
	g++ -o $(OBJ) -fPIC -shared $(SRC) $(INCLUDE_PATH) $(LIB_PATH) $(LIB) -D $(DEFINE) -std=c++11
	cp ../lib/libtcpModule.so ../test/
	cp ../lib/libtcpModule.so ../bin/
