SRC=$(shell find ./src/ -name *.cpp)

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I ../include/

LIB_PATH=-L ./
LIB_PATH+=-L ../lib
LIB=-levent

DEFINE=DEBUG

OBJ=../lib/libmyEvent.so
OBJ:$(SRC)
	@echo "make libmyEvent.so"
	g++ -o $(OBJ) -fPIC -shared $(SRC) $(INCLUDE_PATH) $(LIB_PATH) $(LIB) -D $(DEFINE) --std=c++11
	cp ../lib/libmyEvent.so ../test/
	cp ../lib/libmyEvent.so ../bin/
