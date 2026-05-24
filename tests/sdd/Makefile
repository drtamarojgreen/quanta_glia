CXX = g++
CXXFLAGS = -std=c++17 -I../../src -I../cpp/util
BUILD_DIR = ../../build
BIN_DIR = bin

CORE_OBJS = $(shell find $(BUILD_DIR)/src -name "*.o" ! -name "main.cpp.o" ! -name "glia_main.cpp.o")
CARDS = cards/GliaAppClass.cpp
# Note: GliaAppClass.cpp includes other cards, or we can compile them separately.
# In our current setup, GliaAppClass.cpp is the runner.

all: $(BIN_DIR)/sorrel_test

$(BIN_DIR)/sorrel_test: cards/GliaAppClass.cpp tests/sdd/cards/EnhancementsClass.cpp tests/sdd/cards/WorkflowClass.cpp tests/sdd/cards/WasteClass.cpp tests/sdd/cards/RobustnessClass.cpp tests/sdd/cards/DynamicCommandClass.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(CORE_OBJS) -o $@

test: all
	@./$(BIN_DIR)/sorrel_test

clean:
	rm -rf $(BIN_DIR)

.PHONY: all test clean
