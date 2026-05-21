# ============================================================
#  VisionLint — Linux Makefile
#  Translated from CodeBlocks / MinGW64 build log
# ============================================================

CXX      := g++
CXXFLAGS := -Wall -std=c++17 -g -fexceptions -Iinclude -MMD

# ── Output names ──────────────────────────────────────────
TEST_BIN := VisionLintTest

# ── Object directories ────────────────────────────────────
OBJ_SRC  := .objs/src
OBJ_TEST := .objs/test

# ── Source files ──────────────────────────────────────────
SRC_FILES := \
	src/analyze_lines.cpp \
	src/environment.cpp \
	src/error_object.cpp \
	src/log_reader.cpp \
	src/produce_resolutions.cpp \
	src/try_alternatives.cpp \
	src/config.cpp \
	src/create_reports.cpp

TEST_FILES := \
	test/test_analyze_lines.cpp \
	test/test_environment.cpp \
	test/test_log_reader.cpp \
	test/test_main.cpp \
	test/test_produce_resolutions.cpp \
	test/test_try_alternatives.cpp \
	test/test_integration.cpp \
	test/test_sample_logs.cpp \
	test/test_helpers.cpp

# ── Derive object file lists ──────────────────────────────
SRC_OBJS  := $(patsubst src/%.cpp,  $(OBJ_SRC)/%.o,  $(SRC_FILES))
TEST_OBJS := $(patsubst test/%.cpp, $(OBJ_TEST)/%.o, $(TEST_FILES))
DEP_FILES := $(SRC_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# ── Default target ────────────────────────────────────────
.PHONY: all
all: test

# ── Test binary ───────────────────────────────────────────
.PHONY: test
test: $(TEST_BIN)

$(TEST_BIN): $(SRC_OBJS) $(TEST_OBJS)
	$(CXX) -o $@ $^

# ── Compile rules ─────────────────────────────────────────
$(OBJ_SRC)/%.o: src/%.cpp | $(OBJ_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_TEST)/%.o: test/%.cpp | $(OBJ_TEST)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Create object directories on demand ───────────────────
$(OBJ_SRC):
	mkdir -p $(OBJ_SRC)

$(OBJ_TEST):
	mkdir -p $(OBJ_TEST)

# ── Run the test binary ───────────────────────────────────
.PHONY: run
run: $(TEST_BIN)
	./$(TEST_BIN)

# ── Clean up build artefacts ──────────────────────────────
.PHONY: clean
clean:
	rm -rf .objs $(TEST_BIN)

-include $(DEP_FILES)
