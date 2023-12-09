.PHONY: build run test

build: clean-build
	mkdir build
	cd ./build; g++ -pthread ../src/main.cpp ../src/utils.cpp -lrt -o main; g++ -pthread ../src/child1.cpp ../src/utils.cpp -lrt -o child1; g++ -pthread ../src/child2.cpp ../src/utils.cpp -lrt -o child2

run:
	cd build; ./main

clean-build:
	rm -rf ./build/