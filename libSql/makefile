SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I../include/

LIB_PATH=-L ./
LIB=-lmysqlclient

DEFINE=DEBUG
OBJ=../lib/libmysql.so
OBJ:$(SRC)
	@echo "make libmyLog.so"
	g++ -o $(OBJ)  -fPIC -shared $(SRC) $(INCLUDE_PATH)  $(LIB_PATH) $(LIB) -D $(DEFINE) -std=c++11
	cp ../lib/libmysql.so ../test/
	cp ../lib/libmysql.so ../bin/
	
