SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I../include/

LIB_PATH=-L ../lib
LIB=-lmyLog
LIB=-llog4cxx


DEFINE=DEBUG
OBJ=../lib/libmyparser.so
OBJ:$(SRC)
	@echo "make parser"
	g++ -o $(OBJ)  -fPIC -shared $(SRC) $(INCLUDE_PATH)  $(LIB_PATH) $(LIB) -D $(DEFINE) -std=c++11
	cp ../lib/libmyparser.so ../test/	
