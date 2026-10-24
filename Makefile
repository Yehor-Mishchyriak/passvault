CXX = c++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

TEST_SOURCES = tests/run_all.cpp \
               tests/test_vault.cpp \
               tests/helpers.cpp \
               src/core/vault.cpp

TEST_HEADERS = tests/test_vault.hpp \
               tests/helpers.hpp \
               src/core/vault.hpp

.PHONY: test

test: build/run_all
	./build/run_all

build/run_all: $(TEST_SOURCES) $(TEST_HEADERS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $@