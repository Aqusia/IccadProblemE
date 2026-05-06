CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra -pthread

BIN_DIR = bin
INPUT_DIR = input_pa2
OUTPUT_DIR = output_pa2
EXECUTABLE = $(BIN_DIR)/fp
CHECKER = evaluator/checker
EVALUATOR = evaluator/evaluator.sh

SOURCES = src/floorplanner.cpp src/sa.cpp src/sa_support.cpp src/sa_moves.cpp src/main.cpp
OBJECTS = $(BIN_DIR)/floorplanner.o $(BIN_DIR)/sa.o $(BIN_DIR)/sa_support.o $(BIN_DIR)/sa_moves.o $(BIN_DIR)/main.o
INCLUDES = src/floorplanner.h src/module.h src/sa.h
CASES_ALL ?= ami33 ami49 apte hp xerox

CASE ?= ami33
ALPHA ?= 0.5
BLOCK_FILE ?= $(INPUT_DIR)/$(CASE).block
NET_FILE ?= $(INPUT_DIR)/$(CASE).nets
OUTPUT_FILE ?= $(OUTPUT_DIR)/$(CASE).rpt
SVG_FILE ?= $(OUTPUT_DIR)/$(CASE).svg

.PHONY: all run check eval plot plot_all run_eval_all clean

all: $(EXECUTABLE)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

$(BIN_DIR)/floorplanner.o: src/floorplanner.cpp $(INCLUDES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/main.o: src/main.cpp $(INCLUDES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/sa.o: src/sa.cpp $(INCLUDES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/sa_support.o: src/sa_support.cpp $(INCLUDES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/sa_moves.o: src/sa_moves.cpp $(INCLUDES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(OUTPUT_DIR)
	./$(EXECUTABLE) $(ALPHA) $(BLOCK_FILE) $(NET_FILE) $(OUTPUT_FILE)

check: 
	chmod 744 $(CHECKER)
	./$(CHECKER) $(BLOCK_FILE) $(NET_FILE) $(OUTPUT_FILE) $(ALPHA)

eval: 
	bash $(EVALUATOR) $(BLOCK_FILE) $(NET_FILE) $(OUTPUT_FILE) $(ALPHA)

run_eval_all: $(OUTPUT_DIR)
	@for case in $(CASES_ALL); do \
		out="$(OUTPUT_DIR)/$$case.rpt"; \
		echo "=== $$case ==="; \
		./$(EXECUTABLE) $(ALPHA) $(INPUT_DIR)/$$case.block $(INPUT_DIR)/$$case.nets $$out; \
		bash $(EVALUATOR) $(INPUT_DIR)/$$case.block $(INPUT_DIR)/$$case.nets $$out $(ALPHA); \
	done

plot:
	python3 plot_floorplan.py $(OUTPUT_FILE) $(SVG_FILE) --block $(BLOCK_FILE)

plot_all: $(OUTPUT_DIR)
	@for case in $(CASES_ALL); do \
		echo "=== $$case ==="; \
		python3 plot_floorplan.py \
			$(OUTPUT_DIR)/$$case.rpt \
			$(OUTPUT_DIR)/$$case.svg \
			--block $(INPUT_DIR)/$$case.block; \
	done
	
clean:
	rm -f $(BIN_DIR)/floorplanner.o $(BIN_DIR)/sa.o $(BIN_DIR)/sa_support.o $(BIN_DIR)/sa_moves.o $(BIN_DIR)/main.o
	rm -f $(EXECUTABLE) $(EXECUTABLE).exe
	rm -f $(OUTPUT_DIR)/*.rpt $(OUTPUT_DIR)/*.svg
