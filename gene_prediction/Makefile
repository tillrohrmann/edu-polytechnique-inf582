SRC:=$(wildcard *.cpp)

OBJ:=$(SRC:.cpp=.o)

CFLAGS:=-ggdb -O3 -I/opt/local/include
CXXFLAGS:=$(CFLAGS) 
CC:=gcc
CXX:=g++
LDFLAGS:=
LIBS:=-L/opt/local/lib -lboost_regex

OUTPUT:=gp

all: $(OUTPUT)

$(OUTPUT):$(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
	
clean:
	rm -f $(OBJ)
	
clean-all: clean
	rm -f $(OUTPUT)

%.o:%.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $*.o
