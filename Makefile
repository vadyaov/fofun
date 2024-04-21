
install:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build

make run:
	./build/bin/forfun

clean:
	rm -rf build

.PHONY: install build clean
