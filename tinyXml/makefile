SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I../include/
INCLUDE_PATH+= -I../include/XML/
LIB_PATH=-L ./


DEFINE=DEBUG
OBJ=../lib/libmyXML.so
OBJ:$(SRC)
	@echo "make libmyXML.so"
	g++ -o $(OBJ)  -fPIC -shared $(SRC) $(INCLUDE_PATH)  $(LIB_PATH) $(LIB) -D $(DEFINE) -std=c++11
	cp ../lib/libmyXML.so ../test/
	
