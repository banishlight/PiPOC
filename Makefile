CXX:=g++
CXXFLAGS:= -Wall -Wextra -std=c++20 -MMD
LDFLAGS:= -lraylib -lm -lpthread
SRC_DIR:=src
INC_DIR:=inc
BUILD_DIR:=build
BIN_DIR:=bin
TARGET:=$(BIN_DIR)/pipoc

SRC:=$(shell find $(SRC_DIR) -name '*.cpp')
OBJ:=$(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS:=$(OBJ:.o=.d)

.PHONY: default clean directories

default: directories $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	cp -r assets $(BIN_DIR)/

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

directories:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)