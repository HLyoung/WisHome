SRC=$(shell find ./src/ -name "*.cpp")

INCLUDE_PATH=-I./
INCLUDE_PATH+= -I./include/
INCLUDE_PATH+= -I../include/

OBJ=../lib/libtimer.so
OBJ:$(SRC)
	@echo "make timer main"
	g++ -o $(OBJ)  -fPIC -shared  $(SRC) $(INCLUDE_PATH) -std=c++11 -lpthread
	cp $(OBJ) ../test
	cp $(OBJ) ../bin
