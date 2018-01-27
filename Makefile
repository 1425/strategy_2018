CXX=g++-7
CC=$(CXX)
CXXFLAGS=-std=c++17 -Wall -Werror -Ofast -flto

all: 1 max_value stream adverserial

1: util.o state.o limits.o

max_value: state.o util.o limits.o placement.o

adverserial: state.o placement.o util.o

.PHONY: clean
clean:
	rm -f 1 gmon.out max_value *.o stream adverserial
