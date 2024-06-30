main:
	clang -Wall -DDISABLE_SST -std=c++17 main.cpp ./suite/*.cpp ../src/data_movement/*.cpp ../src/top/*.cpp ../src/middle-racer/*.cpp ../src/middle-mimdram/*.cpp ../src/bottom/*.cpp -o gtest -lgtest -lgtest_main -pthread -lpugixml -lstdc++ -lm -lpython3.8 -I /usr/include/python3.8

clean:
	rm -rf *out*
	rm -rf ./gtest.dSYM
	rm gtest
	rm -rf xml_results/*
