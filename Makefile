CXX=g++-7
#CC=$(CXX)
CXXFLAGS=-std=c++17 -Wall -Wextra -Werror -march=native -mtune=native -Ofast -flto

all: 1 max_value stream adverserial skellam pick

1: util.o state.o limits.o

max_value: state.o util.o limits.o placement.o

adverserial: state.o placement.o util.o limits.o

stream: util.o

outline: util.o

auto: util.o

skellam:
	#$(CXX) $(CXXFLAGS) -I/usr/include/python3.6m skellam.cpp -o $@ $(shell python3.6m-config --ldflags)
	$(CXX) $(CXXFLAGS) -I/usr/include/python3.5m skellam.cpp -o $@ $(shell python3.6m-config --ldflags)

pick: util.o sub.o

.PHONY: clean
clean:
	rm -f 1 gmon.out max_value *.o stream adverserial skellam pick
