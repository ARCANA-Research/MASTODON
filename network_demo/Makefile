CXX=clang++
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS) $(PY_CFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)
SOURCE=../src/sst/*.cpp ../src/top/*.cpp ../src/data_movement/*.cpp ../src/middle-racer/*.cpp ../src/bottom/*.cpp
all: libmastodon.so install 

libmastodon.so:
	$(CXX) $(CXXFLAGS) -o libmastodon.so $(SOURCE) $(LDFLAGS) -I../../src/ -I$(ELEMFLAGS) -lpugixml

install:
	sst-register mastodon mastodon_LIBDIR=$(CURDIR)
	sst-register SST_ELEMENT_SOURCE mastodon=$(CURDIR)
	sst-register SST_ELEMENT_TESTS  mastodon=$(CURDIR)

clean:
	rm -rf *.so *out */a.out
	rm -rf *.o
	rm -rf *.xml
	rm -rf __pycache__

PY_CFLAGS  := $(shell python3-config --cflags)
PY_LDFLAGS := $(shell python3-config --embed --ldflags)
