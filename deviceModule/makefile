SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I../include/
INCLUDE_PATH+= -I../include/XML/

LIB_PATH=-L ./
LIB_PATH=-L ../lib
LIB=-llog4cxx
LIB=-lmyLog
LIB=-lmysql
LIB=-ltcpModule
LIB=-lmyXML

DEFINE=DEBUG
OBJ=../lib/libmydevice.so
OBJ:$(SRC)
	@echo "make libmydevice.so"
	g++ -o -g  $(OBJ)  -fPIC -shared $(SRC) $(INCLUDE_PATH)  $(LIB_PATH) $(LIB) -D $(DEFINE) -std=c++11
	cp ../lib/libmydevice.so ../test/
