TARGET = todo_app
HAS_GXX := $(shell which g++ 2>/dev/null)
ROCM_CLANGXX := /usr/lib64/rocm/llvm/bin/clang++

ifneq ($(HAS_GXX),)
    CXX = g++
    CXXFLAGS = -Wall -Wextra -std=c++17 -O2
    SRC = main.cpp
    BUILD_CMD = $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
else ifneq ($(wildcard $(ROCM_CLANGXX)),)
    CXX = $(ROCM_CLANGXX)
    CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -stdlib=libc++ -I/usr/lib64/rocm/llvm/include/c++/v1 -L/usr/lib64/rocm/llvm/lib -Wl,-rpath,/usr/lib64/rocm/llvm/lib
    SRC = main.cpp
    BUILD_CMD = $(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
else
    CC = gcc
    CFLAGS = -Wall -Wextra -std=c11 -O2
    SRC = main.c
    BUILD_CMD = $(CC) $(CFLAGS) -o $(TARGET) $(SRC)
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(BUILD_CMD)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) tasks.txt

.PHONY: all run clean
