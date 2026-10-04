CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Wpedantic

.PHONY: all clean
all: satellite-orbit

satellite-orbit: main.cpp functions.cpp satellite.h
	$(CXX) $(CXXFLAGS) main.cpp functions.cpp -o $@

clean:
	rm -f satellite-orbit satellite-orbit.exe
