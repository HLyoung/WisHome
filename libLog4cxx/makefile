SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I../include/
INCLUDE_PATH+= -I /usr/local/log4cxx/include/

LIB_PATH=-L ./
LIB_PATH+=-L /usr/local/log4cxx/lib/
LIB=-llog4cxx

DEFINE=DEBUG    
OBJ=../lib/libmyLog.so
OBJ:$(SRC)
	@echo "make libmyLog.so"
	g++ -o $(OBJ)  -fPIC -shared $(SRC) $(INCLUDE_PATH)  $(LIB_PATH) $(LIB) -D $(DEFINE)  -std=c++11
	cp ../lib/libmyLog.so ../test/
	cp ../lib/libmyLog.so ../bin/	

