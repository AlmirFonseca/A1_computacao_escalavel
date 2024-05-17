VERSION := 1.55.0
INSTALL_DIR := $(HOME)/.local
NUM_JOBS := 4


build-server-debug:
	mkdir -p ETL/proto && \
	mkdir -p build && \
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g" -Wno-dev && \
	make -j $(NUM_JOBS)

build-server-release:
	mkdir -p ETL/proto && \
	mkdir -p build && \
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3" -Wno-dev && \
	make -j $(NUM_JOBS)

start-server:
	./server