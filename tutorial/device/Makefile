CXX=clang++
CXXFLAGS=$(PY_CFLAGS) -DDISABLE_SST -g
LDFLAGS=$(PY_LDFLAGS)
SOURCE=main.cpp ../../src/top/*.cpp ../../src/data_movement/*.cpp ../../src/middle-racer/*.cpp ../../src/bottom/*.cpp

mastodon:
	$(CXX) $(CXXFLAGS) -o mastodon $(SOURCE) $(LDFLAGS) -I../src/ -lpugixml

clean:
	rm -rf *.so *out */a.out
	rm -rf *.o
	rm -rf *.xml
	rm -rf __pycache__

PY_CFLAGS  := $(shell python3-config --cflags)
PY_LDFLAGS := $(shell python3-config --embed --ldflags)
