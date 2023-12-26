.PHONY: build run test

build: clean-build
	mkdir build
	cd ./build; cmake ..; make all

run:
	./build/*_exe

test:
	./build/*_test

clean-build:
	rm -rf ./build/